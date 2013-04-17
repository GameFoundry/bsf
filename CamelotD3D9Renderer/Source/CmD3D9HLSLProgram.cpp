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
#include "CmGpuParams.h"
#include "CmD3D9HLSLParamParser.h"
#include "CmD3D9HLSLProgramRTTI.h"

namespace CamelotFramework {
	//-----------------------------------------------------------------------
	D3D9HLSLProgram::D3D9HLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, const vector<HGpuProgInclude>::type* includes, bool isAdjacencyInfoRequired)
		: HighLevelGpuProgram(source, entryPoint, language, gptype, profile, includes, isAdjacencyInfoRequired)
		, mPreprocessorDefines()
		, mColumnMajorMatrices(true)
		, mpMicroCode(NULL)
		, mOptimisationLevel(OPT_DEFAULT)
	{
	}
	//-----------------------------------------------------------------------
	D3D9HLSLProgram::~D3D9HLSLProgram()
	{

	}
	//-----------------------------------------------------------------------
	void D3D9HLSLProgram::initialize_internal()
	{
		if (isSupported())
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
			LPD3DXCONSTANTTABLE constTable;

			String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

			// Compile & assemble into microcode
			HRESULT hr = D3DXCompileShader(
				mSource.c_str(),
				static_cast<UINT>(mSource.length()),
				pDefines,
				nullptr, 
				mEntryPoint.c_str(),
				hlslProfile.c_str(),
				compileFlags,
				&mpMicroCode,
				&errors,
				&constTable);

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

			hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

			// Create a low-level program, give it the same name as us
			mAssemblerProgram = 
				GpuProgramManager::instance().createProgram(
				"",// dummy source, since we'll be using microcode
				"",
				hlslProfile,
				mType, 
				GPP_NONE);
			static_cast<D3D9GpuProgram*>(mAssemblerProgram.get())->setExternalMicrocode(mpMicroCode);

			D3D9HLSLParamParser paramParser(constTable);
			mParametersDesc = paramParser.buildParameterDescriptions();

			SAFE_RELEASE(constTable);
		}

		HighLevelGpuProgram::initialize_internal();
	}
    //-----------------------------------------------------------------------
    void D3D9HLSLProgram::destroy_internal()
    {
        SAFE_RELEASE(mpMicroCode);
        
		HighLevelGpuProgram::destroy_internal();
    }
	//-----------------------------------------------------------------------
	LPD3DXBUFFER D3D9HLSLProgram::getMicroCode()
	{
		return mpMicroCode;
	}
    //-----------------------------------------------------------------------
    bool D3D9HLSLProgram::isSupported(void) const
    {
        if (!isRequiredCapabilitiesSupported())
            return false;

		String hlslProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);

		RenderSystem* rs = CamelotFramework::RenderSystem::instancePtr();
		return rs->getCapabilities()->isShaderProfileSupported(hlslProfile);
    }
	//-----------------------------------------------------------------------
	GpuParamsPtr D3D9HLSLProgram::createParameters()
	{
		GpuParamsPtr params(CM_NEW(GpuParams, PoolAlloc) GpuParams(mParametersDesc),
			&MemAllocDeleter<GpuParams, PoolAlloc>::deleter);
		params->setTransposeMatrices(mColumnMajorMatrices);

		return params;
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
