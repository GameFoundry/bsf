#include "CmD3D11HLSLProgram.h"
#include "CmD3D11HLSLProgramRTTI.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramManager.h"
#include "CmD3D11GpuProgram.h"
#include "CmHardwareBufferManager.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11HLSLParamParser.h"
#include "CmD3D11Mappings.h"
#include "CmGpuParams.h"
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
		String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

		ID3DBlob* microcode = compileMicrocode(hlslProfile);

		mMicrocode.resize(microcode->GetBufferSize());
		memcpy(&mMicrocode[0], microcode->GetBufferPointer(), microcode->GetBufferSize());

		populateParametersAndConstants(microcode);

		mAssemblerProgram = GpuProgramManager::instance().createProgram("", "", hlslProfile, mType, GPP_NONE); // We load it from microcode, so none of this matters
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());

		switch(mType)
		{
		case GPT_VERTEX_PROGRAM:
			{
				D3D11GpuVertexProgramPtr vertProgram = std::static_pointer_cast<D3D11GpuVertexProgram>(mAssemblerProgram);
				vertProgram->loadFromMicrocode(rs->getPrimaryDevice(), microcode);
			}
			break;
		case GPT_FRAGMENT_PROGRAM:
			{
				D3D11GpuFragmentProgramPtr fragProgram = std::static_pointer_cast<D3D11GpuFragmentProgram>(mAssemblerProgram);
				fragProgram->loadFromMicrocode(rs->getPrimaryDevice(), microcode);
			}
			break;
		case GPT_GEOMETRY_PROGRAM:
			{
				D3D11GpuGeometryProgramPtr geomProgram = std::static_pointer_cast<D3D11GpuGeometryProgram>(mAssemblerProgram);
				geomProgram->loadFromMicrocode(rs->getPrimaryDevice(), microcode);
			}
			break;
		case GPT_HULL_PROGRAM:
			{
				D3D11GpuHullProgramPtr hullProgram = std::static_pointer_cast<D3D11GpuHullProgram>(mAssemblerProgram);
				hullProgram->loadFromMicrocode(rs->getPrimaryDevice(), microcode);
			}
			break;
		case GPT_DOMAIN_PROGRAM:
			{
				D3D11GpuDomainProgramPtr domainProgram = std::static_pointer_cast<D3D11GpuDomainProgram>(mAssemblerProgram);
				domainProgram->loadFromMicrocode(rs->getPrimaryDevice(), microcode);
			}
			break;
		}

		SAFE_RELEASE(microcode);
	}

    void D3D11HLSLProgram::unload_internal()
	{
		mAssemblerProgram = nullptr;
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

	ID3DBlob* D3D11HLSLProgram::compileMicrocode(const String& profile)
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
			profile.c_str(),// [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
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

		D3D11HLSLParamParser parser;

		if(mType == GPT_VERTEX_PROGRAM)
			mInputDeclaration = HardwareBufferManager::instance().createVertexDeclaration();

		parser.parse(microcode, mParametersDesc, mInputDeclaration);
	}

	GpuParamsPtr D3D11HLSLProgram::createParameters()
	{
		GpuParamsPtr params(new GpuParams(mParametersDesc));
		params->setTransposeMatrices(mColumnMajorMatrices);

		return params;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* D3D11HLSLProgram::getRTTIStatic()
	{
		return D3D11HLSLProgramRTTI::instance();
	}

	RTTITypeBase* D3D11HLSLProgram::getRTTI() const
	{
		return D3D11HLSLProgram::getRTTIStatic();
	}
}