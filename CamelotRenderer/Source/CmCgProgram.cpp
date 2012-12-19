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
#include "CmCgProgram.h"
#include "CmGpuProgramManager.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmCgProgramRTTI.h"

namespace CamelotEngine {

	void checkForCgError(const String& ogreMethod, const String& errorTextPrefix, CGcontext context)
	{
		CGerror error = cgGetError();
		if (error != CG_NO_ERROR)
		{
			String msg = errorTextPrefix + cgGetErrorString(error); 

			if (error == CG_COMPILER_ERROR)
			{
				// Get listing with full compile errors
				msg = msg + "\n" + cgGetLastListing(context);
			}

			CM_EXCEPT(InternalErrorException, msg);
		}
	}
    //-----------------------------------------------------------------------
    void CgProgram::selectProfile(void)
    {
        mSelectedProfile.clear();
        mSelectedCgProfile = CG_PROFILE_UNKNOWN;

		mSelectedProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);
		GpuProgramManager& gpuMgr = GpuProgramManager::instance();
		if (gpuMgr.isSyntaxSupported(mSelectedProfile))
		{
			mSelectedCgProfile = cgGetProfile(mSelectedProfile.c_str());
			// Check for errors
			checkForCgError("CgProgram::selectProfile", 
				"Unable to find CG profile enum for program.", mCgContext);
		}
		else
			mSelectedProfile.clear();
    }
    //-----------------------------------------------------------------------
    void CgProgram::buildArgs(void)
    {
        vector<String>::type args;
        if (!mCompileArgs.empty())
            args = StringUtil::split(mCompileArgs);

        vector<String>::type::const_iterator i;
        if (mSelectedCgProfile == CG_PROFILE_VS_1_1)
        {
            // Need the 'dcls' argument whenever we use this profile
            // otherwise compilation of the assembler will fail
            bool dclsFound = false;
            for (i = args.begin(); i != args.end(); ++i)
            {
                if (*i == "dcls")
                {
                    dclsFound = true;
                    break;
                }
            }
            if (!dclsFound)
            {
                args.push_back("-profileopts");
				args.push_back("dcls");
            }
        }
        // Now split args into that god-awful char** that Cg insists on
        freeCgArgs();
        mCgArguments = (char**)malloc(sizeof(char*) * (args.size() + 1));
        int index = 0;
        for (i = args.begin(); i != args.end(); ++i, ++index)
        {
            mCgArguments[index] = (char*)malloc(sizeof(char) * (i->length() + 1));
            strcpy(mCgArguments[index], i->c_str());
        }
        // Null terminate list
        mCgArguments[index] = 0;


    }
    //-----------------------------------------------------------------------
    void CgProgram::freeCgArgs(void)
    {
        if (mCgArguments)
        {
            size_t index = 0;
            char* current = mCgArguments[index];
            while (current)
            {
				free(current);
				mCgArguments[index] = 0;
                current = mCgArguments[++index];
            }
			free(mCgArguments);
            mCgArguments = 0;
        }
    }
    //-----------------------------------------------------------------------
    void CgProgram::loadFromSource(void)
    {
        // Create Cg Program
        selectProfile();
		if (mSelectedCgProfile == CG_PROFILE_UNKNOWN)
		{
			gDebug().log("Attempted to load Cg program but no supported "
				"profile was found.", "RenderSystem");
			return;
		}
        buildArgs();

		// TODO PORT - This doesn't load includes
		// deal with includes
		String sourceToUse = mSource;
		//String sourceToUse = resolveCgIncludes(mSource, this, mFilename);
        mCgProgram = cgCreateProgram(mCgContext, CG_SOURCE, sourceToUse.c_str(), 
            mSelectedCgProfile, mEntryPoint.c_str(), const_cast<const char**>(mCgArguments));

        // Test
        //LogManager::getSingleton().logMessage(cgGetProgramString(mCgProgram, CG_COMPILED_PROGRAM));

        // Check for errors
        checkForCgError("CgProgram::loadFromSource", 
            "Unable to compile Cg program", mCgContext);

		// ignore any previous error
		if (mSelectedCgProfile != CG_PROFILE_UNKNOWN && !mCompileError)
		{
			if (mSelectedCgProfile == CG_PROFILE_VS_4_0 || mSelectedCgProfile == CG_PROFILE_PS_4_0)
			{
				String hlslSourceFromCg = cgGetProgramString(mCgProgram, CG_COMPILED_PROGRAM);

				// Create a high-level program, give it the same name as us
				HighLevelGpuProgramPtr vp = 
					HighLevelGpuProgramManager::instance().create(
					hlslSourceFromCg, "main", "hlsl", mType, mProfile);

				vp->initialize();

				mAssemblerProgram = vp;
			}
			else
			{

				String shaderAssemblerCode = cgGetProgramString(mCgProgram, CG_COMPILED_PROGRAM);

				// Create a low-level program, give it the same name as us
				mAssemblerProgram = 
					GpuProgramManager::instance().createProgram(
					shaderAssemblerCode, "", mSelectedProfile,
					mType, 
					GPP_NONE);
			}
			// Shader params need to be forwarded to low level implementation
			mAssemblerProgram->setAdjacencyInfoRequired(isAdjacencyInfoRequired());
		}
    }
    //-----------------------------------------------------------------------
    void CgProgram::unload_internal(void)
    {
        // Unload Cg Program
        // Lowlevel program will get unloaded elsewhere
        if (mCgProgram)
        {
            cgDestroyProgram(mCgProgram);
            checkForCgError("CgProgram::unloadImpl", 
                "Error while unloading Cg program", 
                mCgContext);
            mCgProgram = 0;
        }

		HighLevelGpuProgram::unload_internal();
    }
    //-----------------------------------------------------------------------
    void CgProgram::buildConstantDefinitions() const
    {
        // Derive parameter names from Cg
		createParameterMappingStructures(true);

		if (!mCgProgram)
			return;

		recurseParams(cgGetFirstParameter(mCgProgram, CG_PROGRAM));
        recurseParams(cgGetFirstParameter(mCgProgram, CG_GLOBAL));
	}
	//---------------------------------------------------------------------
	void CgProgram::recurseParams(CGparameter parameter, UINT32 contextArraySize) const
	{
		while (parameter != 0)
        {
            // Look for parameters
            // Don't bother enumerating unused parameters, especially since they will
            // be optimised out and therefore not in the indexed versions
            CGtype paramType = cgGetParameterType(parameter);

            if (cgGetParameterVariability(parameter) == CG_UNIFORM &&
                paramType != CG_SAMPLERRECT &&
                cgGetParameterDirection(parameter) != CG_OUT && 
                cgIsParameterReferenced(parameter))
            {
				int arraySize;

				switch(paramType)
				{
				case CG_STRUCT:
					recurseParams(cgGetFirstStructParameter(parameter));
					break;
				case CG_ARRAY:
					// Support only 1-dimensional arrays
					arraySize = cgGetArraySize(parameter, 0);
					recurseParams(cgGetArrayParameter(parameter, 0), (size_t)arraySize);
					break;
				default:
					// Normal path (leaf)
					String paramName = cgGetParameterName(parameter);
					UINT32 logicalIndex = (UINT32)cgGetParameterResourceIndex(parameter);

					// Get the parameter resource, to calculate the physical index
					CGresource res = cgGetParameterResource(parameter);
					bool isRegisterCombiner = false;
					UINT32 regCombinerPhysicalIndex = 0;
					switch (res)
					{
					case CG_COMBINER_STAGE_CONST0:
						// register combiner, const 0
						// the index relates to the texture stage; store this as (stage * 2) + 0
						regCombinerPhysicalIndex = logicalIndex * 2;
						isRegisterCombiner = true;
						break;
					case CG_COMBINER_STAGE_CONST1:
						// register combiner, const 1
						// the index relates to the texture stage; store this as (stage * 2) + 1
						regCombinerPhysicalIndex = (logicalIndex * 2) + 1;
						isRegisterCombiner = true;
						break;
					default:
						// normal constant
						break;
					}

					// Trim the '[0]' suffix if it exists, we will add our own indexing later
					if (StringUtil::endsWith(paramName, "[0]", false))
					{
						paramName.erase(paramName.size() - 3);
					}


					GpuConstantDefinition def;
					def.arraySize = contextArraySize;
					mapTypeAndElementSize(paramType, isRegisterCombiner, def);

					if (def.constType == GCT_UNKNOWN)
					{
						gDebug().log("Problem parsing the following Cg Uniform: '" + paramName + "'", "RenderSystem");
						// next uniform
						parameter = cgGetNextParameter(parameter);
						continue;
					}
					if (isRegisterCombiner)
					{
						def.physicalIndex = regCombinerPhysicalIndex;
					}
					else
					{
						// base position on existing buffer contents
						if(def.isSampler())
						{
							def.physicalIndex = mSamplerLogicalToPhysical->bufferSize;
						}
						else
						{
							if (def.isFloat())
							{
								def.physicalIndex = mFloatLogicalToPhysical->bufferSize;
							}
							else
							{
								def.physicalIndex = mIntLogicalToPhysical->bufferSize;
							}
						}
					}

					def.logicalIndex = logicalIndex;
					mConstantDefs->map.insert(GpuConstantDefinitionMap::value_type(paramName, def));

					// Record logical / physical mapping
					if(def.isSampler())
					{
						mSamplerLogicalToPhysical->map.insert(
							GpuLogicalIndexUseMap::value_type(logicalIndex, 
							GpuLogicalIndexUse(def.physicalIndex, def.arraySize, GPV_GLOBAL)));
						mSamplerLogicalToPhysical->bufferSize += def.arraySize;
						mConstantDefs->samplerCount = mSamplerLogicalToPhysical->bufferSize;

						mTextureLogicalToPhysical->map.insert(
							GpuLogicalIndexUseMap::value_type(logicalIndex, 
							GpuLogicalIndexUse(def.physicalIndex, def.arraySize, GPV_GLOBAL)));
						mTextureLogicalToPhysical->bufferSize += def.arraySize;
						mConstantDefs->textureCount = mTextureLogicalToPhysical->bufferSize;
					}
					else
					{
						if (def.isFloat())
						{
							mFloatLogicalToPhysical->map.insert(
								GpuLogicalIndexUseMap::value_type(logicalIndex, 
								GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
							mFloatLogicalToPhysical->bufferSize += def.arraySize * def.elementSize;
							mConstantDefs->floatBufferSize = mFloatLogicalToPhysical->bufferSize;
						}
						else
						{
							mIntLogicalToPhysical->map.insert(
								GpuLogicalIndexUseMap::value_type(logicalIndex, 
								GpuLogicalIndexUse(def.physicalIndex, def.arraySize * def.elementSize, GPV_GLOBAL)));
							mIntLogicalToPhysical->bufferSize += def.arraySize * def.elementSize;
							mConstantDefs->intBufferSize = mIntLogicalToPhysical->bufferSize;
						}
					}

					// Deal with array indexing
					mConstantDefs->generateConstantDefinitionArrayEntries(paramName, def);

					break;
		
				}
					
            }
            // Get next
            parameter = cgGetNextParameter(parameter);
        }

        
    }
	//-----------------------------------------------------------------------
	void CgProgram::mapTypeAndElementSize(CGtype cgType, bool isRegisterCombiner, 
		GpuConstantDefinition& def) const
	{
		if (isRegisterCombiner)
		{
			// register combiners are the only single-float entries in our buffer
			def.constType = GCT_FLOAT1;
			def.elementSize = 1;
		}
		else
		{
			switch(cgType)
			{
			case CG_SAMPLER1D:
				def.constType = GCT_SAMPLER1D;
				break;
			case CG_SAMPLER2D:
				def.constType = GCT_SAMPLER2D;
				break;
			case CG_SAMPLER3D:
				def.constType = GCT_SAMPLER3D;
				break;
			case CG_SAMPLERCUBE:
				def.constType = GCT_SAMPLERCUBE;
				break;
			case CG_FLOAT:
			case CG_FLOAT1:
			case CG_HALF:
			case CG_HALF1:
				def.constType = GCT_FLOAT1;
				break;
			case CG_FLOAT2:
			case CG_HALF2:
				def.constType = GCT_FLOAT2;
				break;
			case CG_FLOAT3:
			case CG_HALF3:
				def.constType = GCT_FLOAT3;
				break;
			case CG_FLOAT4:
			case CG_HALF4:
				def.constType = GCT_FLOAT4;
				break;
			case CG_FLOAT2x2:
			case CG_HALF2x2:
				def.constType = GCT_MATRIX_2X2;
				break;
			case CG_FLOAT2x3:
			case CG_HALF2x3:
				def.constType = GCT_MATRIX_2X3;
				break;
			case CG_FLOAT2x4:
			case CG_HALF2x4:
				def.constType = GCT_MATRIX_2X4;
				break;
			case CG_FLOAT3x2:
			case CG_HALF3x2:
				def.constType = GCT_MATRIX_3X2;
				break;
			case CG_FLOAT3x3:
			case CG_HALF3x3:
				def.constType = GCT_MATRIX_3X3;
				break;
			case CG_FLOAT3x4:
			case CG_HALF3x4:
				def.constType = GCT_MATRIX_3X4;
				break;
			case CG_FLOAT4x2:
			case CG_HALF4x2:
				def.constType = GCT_MATRIX_4X2;
				break;
			case CG_FLOAT4x3:
			case CG_HALF4x3:
				def.constType = GCT_MATRIX_4X3;
				break;
			case CG_FLOAT4x4:
			case CG_HALF4x4:
				def.constType = GCT_MATRIX_4X4;
				break;
			case CG_INT:
			case CG_INT1:
				def.constType = GCT_INT1;
				break;
			case CG_INT2:
				def.constType = GCT_INT2;
				break;
			case CG_INT3:
				def.constType = GCT_INT3;
				break;
			case CG_INT4:
				def.constType = GCT_INT4;
				break;
			default:
				def.constType = GCT_UNKNOWN;
				break;
			}
			// Cg pads
			def.elementSize = GpuConstantDefinition::getElementSize(def.constType, true);
		}
	}
    //-----------------------------------------------------------------------
		CgProgram::CgProgram(CGcontext context, const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
        : HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired), 
        mCgContext(context), mCgProgram(0), 
        mSelectedCgProfile(CG_PROFILE_UNKNOWN), mCgArguments(0)
    {

    }
    //-----------------------------------------------------------------------
    CgProgram::~CgProgram()
    {
        freeCgArgs();

        unload_internal();
    }
    //-----------------------------------------------------------------------
    bool CgProgram::isSupported(void) const
    {
        if (mCompileError || !isRequiredCapabilitiesSupported())
            return false;

		String selectedProfile = GpuProgramManager::instance().gpuProgProfileToRSSpecificProfile(mProfile);
		if (GpuProgramManager::instance().isSyntaxSupported(selectedProfile))
			return true;

        return false;
    }
	//-----------------------------------------------------------------------
	String CgProgram::resolveCgIncludes(const String& inSource, Resource* resourceBeingLoaded, const String& fileName)
	{
		String outSource;
		// TODO PORT - Includes are not handled ATM
		// output will be at least this big
		//outSource.reserve(inSource.length());

		//size_t startMarker = 0;
		//size_t i = inSource.find("#include");
		//while (i != String::npos)
		//{
		//	size_t includePos = i;
		//	size_t afterIncludePos = includePos + 8;
		//	size_t newLineBefore = inSource.rfind("\n", includePos);

		//	// check we're not in a comment
		//	size_t lineCommentIt = inSource.rfind("//", includePos);
		//	if (lineCommentIt != String::npos)
		//	{
		//		if (newLineBefore == String::npos || lineCommentIt > newLineBefore)
		//		{
		//			// commented
		//			i = inSource.find("#include", afterIncludePos);
		//			continue;
		//		}

		//	}
		//	size_t blockCommentIt = inSource.rfind("/*", includePos);
		//	if (blockCommentIt != String::npos)
		//	{
		//		size_t closeCommentIt = inSource.rfind("*/", includePos);
		//		if (closeCommentIt == String::npos || closeCommentIt < blockCommentIt)
		//		{
		//			// commented
		//			i = inSource.find("#include", afterIncludePos);
		//			continue;
		//		}

		//	}

		//	// find following newline (or EOF)
		//	size_t newLineAfter = inSource.find("\n", afterIncludePos);
		//	// find include file string container
		//	String endDelimeter = "\"";
		//	size_t startIt = inSource.find("\"", afterIncludePos);
		//	if (startIt == String::npos || startIt > newLineAfter)
		//	{
		//		// try <>
		//		startIt = inSource.find("<", afterIncludePos);
		//		if (startIt == String::npos || startIt > newLineAfter)
		//		{
		//			CM_EXCEPT(InternalErrorException,
		//				"Badly formed #include directive (expected \" or <) in file "
		//				+ fileName + ": " + inSource.substr(includePos, newLineAfter-includePos));
		//		}
		//		else
		//		{
		//			endDelimeter = ">";
		//		}
		//	}
		//	size_t endIt = inSource.find(endDelimeter, startIt+1);
		//	if (endIt == String::npos || endIt <= startIt)
		//	{
		//		CM_EXCEPT(InternalErrorException,
		//			"Badly formed #include directive (expected " + endDelimeter + ") in file "
		//			+ fileName + ": " + inSource.substr(includePos, newLineAfter-includePos));
		//	}

		//	// extract filename
		//	String filename(inSource.substr(startIt+1, endIt-startIt-1));

		//	// open included file
		//	DataStreamPtr resource = ResourceGroupManager::getSingleton().
		//		openResource(filename, resourceBeingLoaded->getGroup(), true, resourceBeingLoaded);

		//	// replace entire include directive line
		//	// copy up to just before include
		//	if (newLineBefore != String::npos && newLineBefore >= startMarker)
		//		outSource.append(inSource.substr(startMarker, newLineBefore-startMarker+1));

		//	size_t lineCount = 0;
		//	size_t lineCountPos = 0;
		//	
		//	// Count the line number of #include statement
		//	lineCountPos = outSource.find('\n');
		//	while(lineCountPos != String::npos)
		//	{
		//		lineCountPos = outSource.find('\n', lineCountPos+1);
		//		lineCount++;
		//	}

		//	// Add #line to the start of the included file to correct the line count
		//	outSource.append("#line 1 \"" + filename + "\"\n");

		//	outSource.append(resource->getAsString());

		//	// Add #line to the end of the included file to correct the line count
		//	outSource.append("\n#line " + toString(lineCount) + 
		//		"\"" + fileName + "\"\n");

		//	startMarker = newLineAfter;

		//	if (startMarker != String::npos)
		//		i = inSource.find("#include", startMarker);
		//	else
		//		i = String::npos;

		//}
		//// copy any remaining characters
		//outSource.append(inSource.substr(startMarker));

		return outSource;
	}
    //-----------------------------------------------------------------------
    const String& CgProgram::getLanguage(void) const
    {
        static const String language = "cg";

        return language;
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* CgProgram::getRTTIStatic()
	{
		return CgProgramRTTI::instance();
	}

	RTTITypeBase* CgProgram::getRTTI() const
	{
		return CgProgram::getRTTIStatic();
	}
}
