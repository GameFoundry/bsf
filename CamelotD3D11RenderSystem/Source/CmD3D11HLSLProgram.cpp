#include "CmD3D11HLSLProgram.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramManager.h"
#include "CmD3D11GpuProgram.h"
#include "CmHardwareBufferManager.h"
#include "CmHardwareConstantBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	D3D11HLSLProgram::D3D11HLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
		: HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired),
		mColumnMajorMatrices(true), mEnableBackwardsCompatibility(false)
	{
	}

	D3D11HLSLProgram::~D3D11HLSLProgram()
	{
		unload_internal();
	}

    void D3D11HLSLProgram::loadFromSource()
	{
		ID3DBlob* microcode = compileMicrocode();

		mMicrocode.resize(microcode->GetBufferSize());
		memcpy(&mMicrocode[0], microcode->GetBufferPointer(), microcode->GetBufferSize());

		populateParametersAndConstants(microcode);

		createConstantBuffers();

		mAssemblerProgram = GpuProgramManager::instance().createProgram("", "", "", mType, GPP_NONE); // We load it from microcode, so none of this matters

		switch(mType)
		{
		case GPT_VERTEX_PROGRAM:
			{
				D3D11GpuVertexProgramPtr vertProgram = std::static_pointer_cast<D3D11GpuVertexProgram>(mAssemblerProgram);
				vertProgram->loadFromMicrocode(D3D11RenderSystem::getPrimaryDevice(), microcode);
			}
			break;
		case GPT_FRAGMENT_PROGRAM:
			{
				D3D11GpuFragmentProgramPtr fragProgram = std::static_pointer_cast<D3D11GpuFragmentProgram>(mAssemblerProgram);
				fragProgram->loadFromMicrocode(D3D11RenderSystem::getPrimaryDevice(), microcode);
			}
			break;
		case GPT_GEOMETRY_PROGRAM:
			{
				D3D11GpuGeometryProgramPtr geomProgram = std::static_pointer_cast<D3D11GpuGeometryProgram>(mAssemblerProgram);
				geomProgram->loadFromMicrocode(D3D11RenderSystem::getPrimaryDevice(), microcode);
			}
			break;
		case GPT_HULL_PROGRAM:
			{
				D3D11GpuHullProgramPtr hullProgram = std::static_pointer_cast<D3D11GpuHullProgram>(mAssemblerProgram);
				hullProgram->loadFromMicrocode(D3D11RenderSystem::getPrimaryDevice(), microcode);
			}
			break;
		case GPT_DOMAIN_PROGRAM:
			{
				D3D11GpuDomainProgramPtr domainProgram = std::static_pointer_cast<D3D11GpuDomainProgram>(mAssemblerProgram);
				domainProgram->loadFromMicrocode(D3D11RenderSystem::getPrimaryDevice(), microcode);
			}
			break;
		}

		SAFE_RELEASE(microcode);
	}

    void D3D11HLSLProgram::unload_internal()
	{
		mAssemblerProgram = nullptr;
		mShaderBuffers.clear();
		mInputParameters.clear();
		mOutputParameters.clear();
		mConstantBuffers.clear();
		mMicrocode.clear();
	}

	const String& D3D11HLSLProgram::getLanguage() const
	{
		static String name = "hlsl";

		return name;
	}

	bool D3D11HLSLProgram::isSupported() const
	{
		RenderSystem* rs = RenderSystem::instancePtr();

		return rs->getCapabilities()->isShaderProfileSupported(getSyntaxCode()) && HighLevelGpuProgram::isSupported();
	}

	ID3DBlob* D3D11HLSLProgram::compileMicrocode()
	{
		// TODO - Preprocessor defines aren't supported

		UINT compileFlags = 0;
#if defined(CM_DEBUG_MODE)
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		if (mColumnMajorMatrices)
			compileFlags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
		else
			compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

		if (mEnableBackwardsCompatibility)
			compileFlags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

		ID3DBlob* microCode = nullptr;
		ID3DBlob* errors = nullptr;

		HRESULT hr = D3DCompile(
			mSource.c_str(),	// [in] Pointer to the shader in memory. 
			mSource.size(),		// [in] Size of the shader in memory.  
			nullptr,			// [in] The name of the file that contains the shader code. 
			nullptr,			// [in] Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			nullptr,			// [in] Optional. Pointer to an ID3DInclude Interface interface for handling include files. Setting this to NULL will cause a compile error if a shader contains a #include. 
			mEntryPoint.c_str(),// [in] Name of the shader-entrypoint function where shader execution begins. 
			mSyntaxCode.c_str(),// [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			compileFlags,		// [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			0,					// [in] Effect compile flags
			&microCode,			// [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errors				// [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
			);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
				String(static_cast<const char*>(errors->GetBufferPointer()));

			SAFE_RELEASE(errors);
			CM_EXCEPT(RenderingAPIException, message);
		}

		SAFE_RELEASE(errors);

		return microCode;
	}

	void D3D11HLSLProgram::populateParametersAndConstants(ID3DBlob* microcode)
	{
		assert(microcode != nullptr);

		mShaderBuffers.clear();
		mInputParameters.clear();
		mOutputParameters.clear();

		const char* commentString = nullptr;
		ID3DBlob* pIDisassembly = nullptr;
		char* pDisassembly = nullptr;

		HRESULT hr = D3DDisassemble((UINT*)microcode->GetBufferPointer(), 
			microcode->GetBufferSize(), D3D_DISASM_ENABLE_COLOR_CODE, commentString, &pIDisassembly);

		const char* assemblyCode =  static_cast<const char*>(pIDisassembly->GetBufferPointer());

		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Unable to disassemble shader.");

		ID3D11ShaderReflection* shaderReflection;
		hr = D3DReflect((void*)microcode->GetBufferPointer(), microcode->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)&shaderReflection);

		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Cannot reflect D3D11 high-level shader.");

		D3D11_SHADER_DESC shaderDesc;
		hr = shaderReflection->GetDesc(&shaderDesc);

		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Cannot reflect D3D11 high-level shader.");

		mInputParameters.resize(shaderDesc.InputParameters);
		for (UINT32 i = 0; i < shaderDesc.InputParameters; i++)
			shaderReflection->GetInputParameterDesc(i, &(mInputParameters[i]));

		mOutputParameters.resize(shaderDesc.OutputParameters);
		for (UINT32 i = 0; i < shaderDesc.OutputParameters; i++)
			shaderReflection->GetOutputParameterDesc(i, &(mOutputParameters[i]));

		mShaderBuffers.resize(shaderDesc.ConstantBuffers);
		for(UINT32 i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			ID3D11ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer;
			shaderReflectionConstantBuffer = shaderReflection->GetConstantBufferByIndex(i);

			populateConstantBufferParameters(shaderReflectionConstantBuffer);
		}

		shaderReflection->Release();
	}

	void D3D11HLSLProgram::buildConstantDefinitions() const
	{
		createParameterMappingStructures(true);

		for(auto shaderBufferIter = mShaderBuffers.begin(); shaderBufferIter != mShaderBuffers.end(); ++shaderBufferIter)
		{
			for(size_t i = 0; i < shaderBufferIter->variables.size(); i++)
			{
				const D3D11_SHADER_VARIABLE_DESC& variableDesc = shaderBufferIter->variables[i];
				const D3D11_SHADER_TYPE_DESC& variableType = shaderBufferIter->variableTypes[i];

				String name = variableDesc.Name;
				if (name.at(0) == '$')
					name.erase(name.begin());

				// Also trim the '[0]' suffix if it exists, we will add our own indexing later
				if (StringUtil::endsWith(name, "[0]", false))
					name.erase(name.size() - 3);

				UINT32 paramIndex = (UINT32)i;

				// TODO - Need to add support for more types. ESPECIALLY TEXTURES & STRUCTS!
				if(variableType.Type == D3D_SVT_FLOAT || variableType.Type == D3D_SVT_INT || variableType.Type == D3D_SVT_BOOL || variableType.Type == D3D_SVT_SAMPLER1D || 
					variableType.Type == D3D_SVT_SAMPLER2D || variableType.Type == D3D_SVT_SAMPLER3D || variableType.Type == D3D_SVT_SAMPLERCUBE)
				{
					GpuConstantDefinition def;
					def.logicalIndex = paramIndex;
					// populate type, array size & element size
					populateParameterDefinition(variableDesc, variableType, def);

					if(def.isSampler())
					{
						def.physicalIndex = variableDesc.StartSampler;
						mSamplerLogicalToPhysical->map.insert(
							GpuLogicalIndexUseMap::value_type(paramIndex, 
							GpuLogicalIndexUse(def.physicalIndex, def.arraySize, GPV_GLOBAL)));
						mSamplerLogicalToPhysical->bufferSize = std::max(mSamplerLogicalToPhysical->bufferSize, def.physicalIndex + def.arraySize);
						mConstantDefs->samplerCount = mSamplerLogicalToPhysical->bufferSize;

						// TODO - Add textures!
						CM_EXCEPT(NotImplementedException, "Add support for texture parameters!");
					}
					else
					{
						if (def.isFloat())
						{
							def.physicalIndex = variableDesc.StartOffset;
							mFloatLogicalToPhysical->map.insert(
								GpuLogicalIndexUseMap::value_type(paramIndex, 
								GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
							mFloatLogicalToPhysical->bufferSize = std::max(mFloatLogicalToPhysical->bufferSize, def.physicalIndex + def.arraySize);
							mConstantDefs->floatBufferSize = mFloatLogicalToPhysical->bufferSize;
						}
						else
						{
							def.physicalIndex = variableDesc.StartOffset;
							mIntLogicalToPhysical->map.insert(
								GpuLogicalIndexUseMap::value_type(paramIndex, 
								GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
							mIntLogicalToPhysical->bufferSize = std::max(mIntLogicalToPhysical->bufferSize, def.physicalIndex + def.arraySize);
							mConstantDefs->intBufferSize = mIntLogicalToPhysical->bufferSize;
						}
					}

					mConstantDefs->map.insert(GpuConstantDefinitionMap::value_type(name, def));

					// Now deal with arrays
					mConstantDefs->generateConstantDefinitionArrayEntries(name, def);
				}
			}
		}
	}

	void D3D11HLSLProgram::populateConstantBufferParameters(ID3D11ShaderReflectionConstantBuffer* bufferReflection)
	{
		D3D11_SHADER_BUFFER_DESC constantBufferDesc;
		HRESULT hr = bufferReflection->GetDesc(&constantBufferDesc);
		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Failed to retrieve HLSL constant buffer description.");

		if(constantBufferDesc.Type != D3D_CBUFFER_TYPE::D3D_CT_CBUFFER && constantBufferDesc.Type != D3D_CBUFFER_TYPE::D3D_CT_TBUFFER)
		{
			LOGDBG("D3D11 HLSL parsing: Unsupported constant buffer type, skipping. Type: " + toString(constantBufferDesc.Type));
			return;
		}

		mShaderBuffers.push_back(D3D11_ShaderBufferDesc());
		D3D11_ShaderBufferDesc& newShaderBufferDesc = *mShaderBuffers.end();

		for(UINT32 j = 0; j < constantBufferDesc.Variables; j++)
		{
			ID3D11ShaderReflectionVariable* varRef;
			varRef = bufferReflection->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			HRESULT hr = varRef->GetDesc(&varDesc);

			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Failed to retrieve HLSL constant buffer variable description.");

			ID3D11ShaderReflectionType* varRefType;
			varRefType = varRef->GetType();
			D3D11_SHADER_TYPE_DESC varTypeDesc;
			varRefType->GetDesc(&varTypeDesc);

			switch(varTypeDesc.Type)
			{
			case D3D_SVT_FLOAT:
			case D3D_SVT_INT:
			case D3D_SVT_SAMPLER1D:
			case D3D_SVT_SAMPLER2D: 
			case D3D_SVT_SAMPLER3D:
			case D3D_SVT_SAMPLERCUBE: // TODO - Need to add support for other types!
				newShaderBufferDesc.variables.push_back(varDesc);
				newShaderBufferDesc.variableTypes.push_back(varTypeDesc);
			default:
				CM_EXCEPT(RenderingAPIException, "Unsupported shader variable type!");
			}
		}
	}

	void D3D11HLSLProgram::populateParameterDefinition(const D3D11_SHADER_VARIABLE_DESC& paramDesc, const D3D11_SHADER_TYPE_DESC& paramType, GpuConstantDefinition& def) const
	{
		def.arraySize = paramType.Elements + 1;
		switch(paramType.Type)
		{
		case D3D_SVT_SAMPLER1D:
			def.constType = GCT_SAMPLER1D;
			def.elementSize = paramDesc.SamplerSize / def.arraySize;
			break;
		case D3D_SVT_SAMPLER2D:
			CM_EXCEPT(NotImplementedException, "Break here because I want to check what is the elementSize of the sampler. It has to be 1.");

			def.constType = GCT_SAMPLER2D;
			def.elementSize = paramDesc.SamplerSize / def.arraySize;
			break;
		case D3D_SVT_SAMPLER3D:
			def.constType = GCT_SAMPLER3D;
			def.elementSize = paramDesc.SamplerSize / def.arraySize;
			break;
		case D3D_SVT_SAMPLERCUBE:
			def.constType = GCT_SAMPLERCUBE;
			def.elementSize = paramDesc.SamplerSize / def.arraySize;
			break;
		case D3D_SVT_INT:
			switch(paramType.Columns)
			{
			case 1:
				def.constType = GCT_INT1;
				def.elementSize = paramDesc.Size / def.arraySize;
				break;
			case 2:
				def.constType = GCT_INT2;
				def.elementSize = paramDesc.Size / def.arraySize;
				break;
			case 3:
				def.constType = GCT_INT3;
				def.elementSize = paramDesc.Size / def.arraySize;
				break;
			case 4:
				def.constType = GCT_INT4;
				def.elementSize = paramDesc.Size / def.arraySize;
				break;
			} // columns
			break;
		case D3D_SVT_FLOAT:

			CM_EXCEPT(NotImplementedException, "Break here because I want to check if paramDesc.Size is size per element or total size of the array.");

			switch(paramType.Rows)
			{
			case 1:
				switch(paramType.Columns)
				{
				case 1:
					def.constType = GCT_FLOAT1;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 2:
					def.constType = GCT_FLOAT2;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 3:
					def.constType = GCT_FLOAT3;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 4:
					def.constType = GCT_FLOAT4;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				} // columns
				break;
			case 2:
				switch(paramType.Columns)
				{
				case 2:
					def.constType = GCT_MATRIX_2X2;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 3:
					def.constType = GCT_MATRIX_2X3;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 4:
					def.constType = GCT_MATRIX_2X4;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				} // columns
				break;
			case 3:
				switch(paramType.Columns)
				{
				case 2:
					def.constType = GCT_MATRIX_3X2;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 3:
					def.constType = GCT_MATRIX_3X3;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 4:
					def.constType = GCT_MATRIX_3X4;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				} // columns
				break;
			case 4:
				switch(paramType.Columns)
				{
				case 2:
					def.constType = GCT_MATRIX_4X2;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 3:
					def.constType = GCT_MATRIX_4X3;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				case 4:
					def.constType = GCT_MATRIX_4X4;
					def.elementSize = paramDesc.Size / def.arraySize;
					break;
				} // columns
				break;

			} // rows
			break;
		default:
			break;
		};
	}

	void D3D11HLSLProgram::createConstantBuffers()
	{
		mConstantBuffers.clear();

		for(auto shaderBufferIter = mShaderBuffers.begin(); shaderBufferIter != mShaderBuffers.end(); ++shaderBufferIter)
		{
			HardwareConstantBufferPtr constantBuffer = HardwareBufferManager::instance().createConstantBuffer(shaderBufferIter->desc.Size, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
			mConstantBuffers.push_back(constantBuffer);
		}
	}
}