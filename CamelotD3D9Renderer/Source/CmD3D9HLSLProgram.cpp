/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmD3D9HLSLProgram.h"
#include "CmGpuProgramManager.h"
#include "CmD3D9GpuProgram.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"
#include "CmD3D9HLSLProgramRTTI.h"

namespace CamelotEngine {
	class CM_D3D9_EXPORT HLSLIncludeHandler : public ID3DXInclude
	{
	public:
		HLSLIncludeHandler(HighLevelGpuProgram* sourceProgram) 
			: mProgram(sourceProgram) {}
		~HLSLIncludeHandler() {}
		
		STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType,
			LPCSTR pFileName,
			LPCVOID pParentData,
			LPCVOID *ppData,
			UINT *pByteLen
			)
		{
			// TODO PORT - I'm not sure what to do with this. It will probably break something in its current state.

			//// find & load source code
			//DataStreamPtr stream = 
			//	ResourceGroupManager::getSingleton().openResource(
			//	String(pFileName), mProgram->getGroup(), true, mProgram);

			//String source = stream->getAsString();
			//// copy into separate c-string
			//// Note - must NOT copy the null terminator, otherwise this will terminate
			//// the entire program string!
			//*pByteLen = static_cast<UINT>(source.length());
			//char* pChar = new char[*pByteLen];
			//memcpy(pChar, source.c_str(), *pByteLen);
			//*ppData = pChar;

			assert(false); // TODO - Include files not supported until I can figure out how to handle them

			return S_OK;
		}

		STDMETHOD(Close)(LPCVOID pData)
		{
			char* pChar = (char*)pData;
			delete [] pChar;
			return S_OK;
		}
	protected:
		HighLevelGpuProgram* mProgram;


	};

	class D3D9HLSLParamParser
	{
	public:
		D3D9HLSLParamParser(LPD3DXCONSTANTTABLE constTable)
			:mpConstTable(constTable), mCurrentBufferSize(0)
		{ }

		GpuParamDesc buildParameterDescriptions();
		void processParameter(GpuParamBlockDesc& blockDesc, D3DXHANDLE parent, String prefix, UINT32 index);
		void populateParamMemberDesc(GpuParamMemberDesc& memberDesc, D3DXCONSTANT_DESC& d3dDesc);

	private:
		LPD3DXCONSTANTTABLE mpConstTable;
		GpuParamDesc mParamDesc;
		UINT32 mCurrentBufferSize;
	};

	GpuParamDesc D3D9HLSLParamParser::buildParameterDescriptions()
	{
		// Derive parameter names from const table
		assert(mpConstTable && "Program not loaded!");

		// Get contents of the constant table
		D3DXCONSTANTTABLE_DESC desc;
		HRESULT hr = mpConstTable->GetDesc(&desc);

		if (FAILED(hr))
			CM_EXCEPT(InternalErrorException, "Cannot retrieve constant descriptions from HLSL program.");
		
		// DX9 has no concept of parameter blocks so we just put all members in one global block
		String name = "Globals";
		mParamDesc.paramBlocks.insert(std::make_pair(name, GpuParamBlockDesc()));
		GpuParamBlockDesc& blockDesc = mParamDesc.paramBlocks[name];
		blockDesc.name = name;
		blockDesc.slot = 0;

		// Iterate over the constants
		for (UINT32 i = 0; i < desc.Constants; ++i)
		{
			// Recursively descend through the structure levels
			processParameter(blockDesc, NULL, "", i);
		}

		return mParamDesc;
	}

	void D3D9HLSLParamParser::processParameter(GpuParamBlockDesc& blockDesc, D3DXHANDLE parent, String prefix, UINT32 index)
	{
		D3DXHANDLE hConstant = mpConstTable->GetConstant(parent, index);

		// Since D3D HLSL doesn't deal with naming of array and struct parameters
		// automatically, we have to do it by hand

		D3DXCONSTANT_DESC desc;
		UINT32 numParams = 1;
		HRESULT hr = mpConstTable->GetConstantDesc(hConstant, &desc, &numParams);
		if (FAILED(hr))
		{
			CM_EXCEPT(InternalErrorException, "Cannot retrieve constant description from HLSL program.");
		}

		String paramName = desc.Name;
		// trim the odd '$' which appears at the start of the names in HLSL
		if (paramName.at(0) == '$')
			paramName.erase(paramName.begin());

		// Also trim the '[0]' suffix if it exists, we will add our own indexing later
		if (StringUtil::endsWith(paramName, "[0]", false))
			paramName.erase(paramName.size() - 3);

		if (desc.Class == D3DXPC_STRUCT)
		{
			// work out a new prefix for nested members, if it's an array, we need an index
			prefix = prefix + paramName + ".";
			// Cascade into struct
			for (UINT32 i = 0; i < desc.StructMembers; ++i)
			{
				processParameter(blockDesc, hConstant, prefix, i);
			}
		}
		else
		{
			// Process params
			if (desc.Type == D3DXPT_FLOAT || desc.Type == D3DXPT_INT || desc.Type == D3DXPT_BOOL)
			{
				GpuParamMemberDesc memberDesc;
				memberDesc.gpuMemOffset = desc.RegisterIndex;
				memberDesc.cpuMemOffset = mCurrentBufferSize;
				memberDesc.paramBlockSlot = blockDesc.slot;
				memberDesc.arraySize = 1;

				String name = prefix + paramName;
				memberDesc.name = name;

				populateParamMemberDesc(memberDesc, desc);
				mParamDesc.params.insert(std::make_pair(name, memberDesc));

				mCurrentBufferSize += memberDesc.elementSize * memberDesc.arraySize;
			}

			if(desc.Type == D3DXPT_SAMPLER1D || desc.Type == D3DXPT_SAMPLER2D || desc.Type == D3DXPT_SAMPLER3D || desc.Type == D3DXPT_SAMPLERCUBE)
			{
				GpuParamSpecialDesc samplerDesc;
				samplerDesc.name = paramName;
				samplerDesc.slot = desc.RegisterIndex;

				GpuParamSpecialDesc textureDesc;
				textureDesc.name = paramName;
				textureDesc.slot = desc.RegisterIndex;

				switch(desc.Type)
				{
				case D3DXPT_SAMPLER1D:
					samplerDesc.type = GST_SAMPLER1D;
					textureDesc.type = GST_TEXTURE1D;
					break;
				case D3DXPT_SAMPLER2D:
					samplerDesc.type = GST_SAMPLER2D;
					textureDesc.type = GST_TEXTURE2D;
					break;
				case D3DXPT_SAMPLER3D:
					samplerDesc.type = GST_SAMPLER3D;
					textureDesc.type = GST_TEXTURE3D;
					break;
				case D3DXPT_SAMPLERCUBE:
					samplerDesc.type = GST_SAMPLERCUBE;
					textureDesc.type = GST_TEXTURECUBE;
					break;
				}

				mParamDesc.samplers.insert(std::make_pair(paramName, samplerDesc));
				mParamDesc.textures.insert(std::make_pair(paramName, textureDesc));
			}
		}
	}

	void D3D9HLSLParamParser::populateParamMemberDesc(GpuParamMemberDesc& memberDesc, D3DXCONSTANT_DESC& d3dDesc)
	{
		memberDesc.arraySize = d3dDesc.Elements;
		switch(d3dDesc.Type)
		{
		case D3DXPT_INT:
			switch(d3dDesc.Columns)
			{
			case 1:
				memberDesc.type = GMT_INT1;
				memberDesc.elementSize = 4;
				break;
			case 2:
				memberDesc.type = GMT_INT2;
				memberDesc.elementSize = 4;
				break;
			case 3:
				memberDesc.type = GMT_INT3;
				memberDesc.elementSize = 4;
				break;
			case 4:
				memberDesc.type = GMT_INT4;
				memberDesc.elementSize = 4;
				break;
			} // columns
			break;
		case D3DXPT_FLOAT:
			switch(d3dDesc.Class)
			{
			case D3DXPC_MATRIX_COLUMNS:
			case D3DXPC_MATRIX_ROWS:
				{
					int firstDim, secondDim;
					firstDim = d3dDesc.RegisterCount / d3dDesc.Elements;

					if (d3dDesc.Class == D3DXPC_MATRIX_ROWS)
						secondDim = d3dDesc.Columns;
					else
						secondDim = d3dDesc.Rows;

					switch(firstDim)
					{
					case 2:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GMT_MATRIX_2X2;
							memberDesc.elementSize = 8; // HLSL always packs
							break;
						case 3:
							memberDesc.type = GMT_MATRIX_2X3;
							memberDesc.elementSize = 8; // HLSL always packs
							break;
						case 4:
							memberDesc.type = GMT_MATRIX_2X4;
							memberDesc.elementSize = 8; 
							break;
						} // columns
						break;
					case 3:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GMT_MATRIX_3X2;
							memberDesc.elementSize = 12; // HLSL always packs
							break;
						case 3:
							memberDesc.type = GMT_MATRIX_3X3;
							memberDesc.elementSize = 12; // HLSL always packs
							break;
						case 4:
							memberDesc.type = GMT_MATRIX_3X4;
							memberDesc.elementSize = 12; 
							break;
						} // columns
						break;
					case 4:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GMT_MATRIX_4X2;
							memberDesc.elementSize = 16; // HLSL always packs
							break;
						case 3:
							memberDesc.type = GMT_MATRIX_4X3;
							memberDesc.elementSize = 16; // HLSL always packs
							break;
						case 4:
							memberDesc.type = GMT_MATRIX_4X4;
							memberDesc.elementSize = 16; 
							break;
						} // secondDim
						break;

					} // firstDim
				}
				break;
			case D3DXPC_SCALAR:
			case D3DXPC_VECTOR:
				switch(d3dDesc.Columns)
				{
				case 1:
					memberDesc.type = GMT_FLOAT1;
					memberDesc.elementSize = 4;
					break;
				case 2:
					memberDesc.type = GMT_FLOAT2;
					memberDesc.elementSize = 4;
					break;
				case 3:
					memberDesc.type = GMT_FLOAT3;
					memberDesc.elementSize = 4;
					break;
				case 4:
					memberDesc.type = GMT_FLOAT4;
					memberDesc.elementSize = 4;
					break;
				} // columns
				break;
			}
		case D3DXPT_BOOL:
			memberDesc.type = GMT_BOOL;
			memberDesc.elementSize = 1;
			break;
		default:
			break;
		};
	}

    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::loadFromSource(void)
    {
        // Populate preprocessor defines
        String stringBuffer;

        vector<D3DXMACRO>::type defines;
        const D3DXMACRO* pDefines = 0;
        if (!mPreprocessorDefines.empty())
        {
            stringBuffer = mPreprocessorDefines;

            // Split preprocessor defines and build up macro array
            D3DXMACRO macro;
            String::size_type pos = 0;
            while (pos != String::npos)
            {
                macro.Name = &stringBuffer[pos];
                macro.Definition = 0;

				String::size_type start_pos=pos;

                // Find delims
                pos = stringBuffer.find_first_of(";,=", pos);

				if(start_pos==pos)
				{
					if(pos==stringBuffer.length())
					{
						break;
					}
					pos++;
					continue;
				}

                if (pos != String::npos)
                {
                    // Check definition part
                    if (stringBuffer[pos] == '=')
                    {
                        // Setup null character for macro name
                        stringBuffer[pos++] = '\0';
                        macro.Definition = &stringBuffer[pos];
                        pos = stringBuffer.find_first_of(";,", pos);
                    }
                    else
                    {
                        // No definition part, define as "1"
                        macro.Definition = "1";
                    }

                    if (pos != String::npos)
                    {
                        // Setup null character for macro name or definition
                        stringBuffer[pos++] = '\0';
                    }
                }
				else
				{
					macro.Definition = "1";
				}
				if(strlen(macro.Name)>0)
				{
					defines.push_back(macro);
				}
				else
				{
					break;
				}
            }

            // Add NULL terminator
            macro.Name = 0;
            macro.Definition = 0;
            defines.push_back(macro);

            pDefines = &defines[0];
        }

        // Populate compile flags
        DWORD compileFlags = 0;
        if (mColumnMajorMatrices)
            compileFlags |= D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
        else
            compileFlags |= D3DXSHADER_PACKMATRIX_ROWMAJOR;

#if CM_DEBUG_MODE
		compileFlags |= D3DXSHADER_DEBUG;
#endif
		switch (mOptimisationLevel)
		{
		case OPT_DEFAULT:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL1;
			break;
		case OPT_NONE:
			compileFlags |= D3DXSHADER_SKIPOPTIMIZATION;
			break;
		case OPT_0:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL0;
			break;
		case OPT_1:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL1;
			break;
		case OPT_2:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL2;
			break;
		case OPT_3:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
			break;
		}


        LPD3DXBUFFER errors = 0;

		// include handler
		HLSLIncludeHandler includeHandler(this);

		String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

        // Compile & assemble into microcode
        HRESULT hr = D3DXCompileShader(
            mSource.c_str(),
            static_cast<UINT>(mSource.length()),
            pDefines,
            &includeHandler, 
            mEntryPoint.c_str(),
            hlslProfile.c_str(),
            compileFlags,
            &mpMicroCode,
            &errors,
            &mpConstTable);

        if (FAILED(hr))
        {
            String message = "Cannot assemble D3D9 high-level shader ";
			
			if( errors )
			{
				message += String(" Errors:\n") + static_cast<const char*>(errors->GetBufferPointer());
				errors->Release();
			}

            CM_EXCEPT(RenderingAPIException, message);
        }

		if (!mCompileError)
		{
			String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

			// Create a low-level program, give it the same name as us
			mAssemblerProgram = 
				GpuProgramManager::instance().createProgram(
				"",// dummy source, since we'll be using microcode
				"",
				hlslProfile,
				mType, 
				GPP_NONE);
			static_cast<D3D9GpuProgram*>(mAssemblerProgram.get())->setExternalMicrocode(mpMicroCode);
		}
    }
    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::unload_internal(void)
    {
        SAFE_RELEASE(mpMicroCode);
        SAFE_RELEASE(mpConstTable);

		HighLevelGpuProgram::unload_internal();
    }
	//-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::buildConstantDefinitions() const
    {
        // Derive parameter names from const table
        assert(mpConstTable && "Program not loaded!");
        // Get contents of the constant table
        D3DXCONSTANTTABLE_DESC desc;
        HRESULT hr = mpConstTable->GetDesc(&desc);

		createParameterMappingStructures(true);

        if (FAILED(hr))
        {
			CM_EXCEPT(InternalErrorException, "Cannot retrieve constant descriptions from HLSL program.");
        }
        // Iterate over the constants
        for (unsigned int i = 0; i < desc.Constants; ++i)
        {
            // Recursively descend through the structure levels
            processParamElement(NULL, "", i);
        }
    }
    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::processParamElement(D3DXHANDLE parent, String prefix, 
        unsigned int index) const
    {
        D3DXHANDLE hConstant = mpConstTable->GetConstant(parent, index);
		
        // Since D3D HLSL doesn't deal with naming of array and struct parameters
        // automatically, we have to do it by hand

        D3DXCONSTANT_DESC desc;
        unsigned int numParams = 1;
        HRESULT hr = mpConstTable->GetConstantDesc(hConstant, &desc, &numParams);
        if (FAILED(hr))
        {
            CM_EXCEPT(InternalErrorException, "Cannot retrieve constant description from HLSL program.");
        }

        String paramName = desc.Name;
        // trim the odd '$' which appears at the start of the names in HLSL
        if (paramName.at(0) == '$')
            paramName.erase(paramName.begin());

		// Also trim the '[0]' suffix if it exists, we will add our own indexing later
		if (StringUtil::endsWith(paramName, "[0]", false))
		{
			paramName.erase(paramName.size() - 3);
		}


        if (desc.Class == D3DXPC_STRUCT)
        {
            // work out a new prefix for nested members, if it's an array, we need an index
            prefix = prefix + paramName + ".";
            // Cascade into struct
            for (unsigned int i = 0; i < desc.StructMembers; ++i)
            {
                processParamElement(hConstant, prefix, i);
            }
        }
        else
        {
            // Process params
            if (desc.Type == D3DXPT_FLOAT || desc.Type == D3DXPT_INT || desc.Type == D3DXPT_BOOL ||
				desc.Type == D3DXPT_SAMPLER1D || desc.Type == D3DXPT_SAMPLER2D || desc.Type == D3DXPT_SAMPLER3D || desc.Type == D3DXPT_SAMPLERCUBE)
            {
                UINT32 paramIndex = desc.RegisterIndex;
                String name = prefix + paramName;
                
				GpuConstantDefinition def;
				def.logicalIndex = paramIndex;
				// populate type, array size & element size
				populateDef(desc, def);

				if(def.isSampler())
				{
					def.physicalIndex = mSamplerLogicalToPhysical->bufferSize;
						mSamplerLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(paramIndex, 
						GpuLogicalIndexUse(def.physicalIndex, def.arraySize, GPV_GLOBAL)));
					mSamplerLogicalToPhysical->bufferSize += def.arraySize;
					mConstantDefs->samplerCount = mSamplerLogicalToPhysical->bufferSize;

						mTextureLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(paramIndex, 
						GpuLogicalIndexUse(def.physicalIndex, def.arraySize, GPV_GLOBAL)));
					mTextureLogicalToPhysical->bufferSize += def.arraySize;
					mConstantDefs->textureCount = mTextureLogicalToPhysical->bufferSize;
				}
				else
				{
					if (def.isFloat())
					{
						def.physicalIndex = mFloatLogicalToPhysical->bufferSize;
							mFloatLogicalToPhysical->map.insert(
							GpuLogicalIndexUseMap::value_type(paramIndex, 
							GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
						mFloatLogicalToPhysical->bufferSize += def.arraySize * def.elementSize;
						mConstantDefs->floatBufferSize = mFloatLogicalToPhysical->bufferSize;
					}
					else
					{
						def.physicalIndex = mIntLogicalToPhysical->bufferSize;
							mIntLogicalToPhysical->map.insert(
							GpuLogicalIndexUseMap::value_type(paramIndex, 
							GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
						mIntLogicalToPhysical->bufferSize += def.arraySize * def.elementSize;
						mConstantDefs->intBufferSize = mIntLogicalToPhysical->bufferSize;
					}
				}

                mConstantDefs->map.insert(GpuConstantDefinitionMap::value_type(name, def));

				// Now deal with arrays
				mConstantDefs->generateConstantDefinitionArrayEntries(name, def);
            }
        }
            
    }
	//-----------------------------------------------------------------------
	void D3D9HLSLProgram::populateDef(D3DXCONSTANT_DESC& d3dDesc, GpuConstantDefinition& def) const
	{
		def.arraySize = d3dDesc.Elements;
		switch(d3dDesc.Type)
		{
		case D3DXPT_SAMPLER1D:
			def.constType = GCT_SAMPLER1D;
			break;
		case D3DXPT_SAMPLER2D:
			def.constType = GCT_SAMPLER2D;
			break;
		case D3DXPT_SAMPLER3D:
			def.constType = GCT_SAMPLER3D;
			break;
		case D3DXPT_SAMPLERCUBE:
			def.constType = GCT_SAMPLERCUBE;
			break;
		case D3DXPT_INT:
			switch(d3dDesc.Columns)
			{
			case 1:
				def.constType = GCT_INT1;
				break;
			case 2:
				def.constType = GCT_INT2;
				break;
			case 3:
				def.constType = GCT_INT3;
				break;
			case 4:
				def.constType = GCT_INT4;
				break;
			} // columns
			break;
		case D3DXPT_FLOAT:
			switch(d3dDesc.Class)
			{
			case D3DXPC_MATRIX_COLUMNS:
			case D3DXPC_MATRIX_ROWS:
				{
					int firstDim, secondDim;
					firstDim = d3dDesc.RegisterCount / d3dDesc.Elements;
					if (d3dDesc.Class == D3DXPC_MATRIX_ROWS)
					{
						secondDim = d3dDesc.Columns;
					}
					else
					{
						secondDim = d3dDesc.Rows;
					}
					switch(firstDim)
					{
					case 2:
						switch(secondDim)
						{
						case 2:
							def.constType = GCT_MATRIX_2X2;
							def.elementSize = 8; // HLSL always packs
							break;
						case 3:
							def.constType = GCT_MATRIX_2X3;
							def.elementSize = 8; // HLSL always packs
							break;
						case 4:
							def.constType = GCT_MATRIX_2X4;
							def.elementSize = 8; 
							break;
						} // columns
						break;
					case 3:
						switch(secondDim)
						{
						case 2:
							def.constType = GCT_MATRIX_3X2;
							def.elementSize = 12; // HLSL always packs
							break;
						case 3:
							def.constType = GCT_MATRIX_3X3;
							def.elementSize = 12; // HLSL always packs
							break;
						case 4:
							def.constType = GCT_MATRIX_3X4;
							def.elementSize = 12; 
							break;
						} // columns
						break;
					case 4:
						switch(secondDim)
						{
						case 2:
							def.constType = GCT_MATRIX_4X2;
							def.elementSize = 16; // HLSL always packs
							break;
						case 3:
							def.constType = GCT_MATRIX_4X3;
							def.elementSize = 16; // HLSL always packs
							break;
						case 4:
							def.constType = GCT_MATRIX_4X4;
							def.elementSize = 16; 
							break;
						} // secondDim
						break;

					} // firstDim
				}
				break;
			case D3DXPC_SCALAR:
			case D3DXPC_VECTOR:
				switch(d3dDesc.Columns)
				{
				case 1:
					def.constType = GCT_FLOAT1;
					break;
				case 2:
					def.constType = GCT_FLOAT2;
					break;
				case 3:
					def.constType = GCT_FLOAT3;
					break;
				case 4:
					def.constType = GCT_FLOAT4;
					break;
				} // columns
				break;
			}
		default:
			// not mapping samplers, don't need to take the space 
			break;
		};

		// D3D9 pads to 4 elements
		def.elementSize = GpuConstantDefinition::getElementSize(def.constType, true);


	}

	LPD3DXBUFFER D3D9HLSLProgram::getMicroCode()
	{
		return mpMicroCode;
	}

    //-----------------------------------------------------------------------
	D3D9HLSLProgram::D3D9HLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
        : HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired)
        , mPreprocessorDefines()
        , mColumnMajorMatrices(true)
        , mpMicroCode(NULL), mpConstTable(NULL)
		, mOptimisationLevel(OPT_DEFAULT)
	{
    }
    //-----------------------------------------------------------------------
    D3D9HLSLProgram::~D3D9HLSLProgram()
    {
        unload_internal();
    }
    //-----------------------------------------------------------------------
    bool D3D9HLSLProgram::isSupported(void) const
    {
        if (mCompileError || !isRequiredCapabilitiesSupported())
            return false;

		String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

		RenderSystem* rs = CamelotEngine::RenderSystem::instancePtr();
		return rs->getCapabilities()->isShaderProfileSupported(hlslProfile);
    }
    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::createParameters_internal(AsyncOp& op)
    {
        // Call superclass
       HighLevelGpuProgram::createParameters_internal(op);

	    GpuProgramParametersSharedPtr params = op.getReturnValue<GpuProgramParametersSharedPtr>();

        // Need to transpose matrices if compiled with column-major matrices
        params->setTransposeMatrices(mColumnMajorMatrices);
    }
    //-----------------------------------------------------------------------
    const String& D3D9HLSLProgram::getLanguage(void) const
    {
        static const String language = "hlsl";

        return language;
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* D3D9HLSLProgram::getRTTIStatic()
	{
		return D3D9HLSLProgramRTTI::instance();
	}

	RTTITypeBase* D3D9HLSLProgram::getRTTI() const
	{
		return D3D9HLSLProgram::getRTTIStatic();
	}
}
