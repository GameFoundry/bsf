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
#include "CmGpuProgram.h"
#include "CmRenderSystem.h"
#include "CmRenderSystemCapabilities.h"
#include "CmGpuProgramManager.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmException.h"

#include "CmGLSLProgram.h"
#include "CmGLSLGpuProgram.h"
#include "CmGLSLExtSupport.h"
#include "CmGLSLLinkProgramManager.h"
#include "CmGLSLPreprocessor.h"

#include "CmGLSLProgramRTTI.h"

namespace CamelotEngine 
{
	class GLSLParamParser
	{
	public:
		void buildUniformDescriptions(GLuint glProgram, GpuParamDesc& returnParamDesc);

	private:
		void determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex);
	};

	void GLSLParamParser::buildUniformDescriptions(GLuint glProgram, GpuParamDesc& returnParamDesc)
	{
		// scan through the active uniforms and add them to the reference list
		GLint maxBufferSize = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxBufferSize);

		GLint maxBlockNameBufferSize = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxBlockNameBufferSize);

		if(maxBlockNameBufferSize > maxBufferSize)
			maxBufferSize = maxBlockNameBufferSize;

		GLchar* uniformName = new GLchar[maxBufferSize];

		GpuParamBlockDesc globalBlockDesc;
		globalBlockDesc.slot = 0;
		globalBlockDesc.name = "CM_INTERNAL_Globals";
		globalBlockDesc.blockSize = 0;

		UINT32 textureSlot = 0;

		returnParamDesc.paramBlocks[globalBlockDesc.name] = globalBlockDesc;

		GLint uniformBlockCount = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);

		map<UINT32, String>::type blockSlotToName;
		for (GLuint index = 0; index < (GLuint)uniformBlockCount; index++)
		{
			GLsizei unusedSize = 0;
			glGetActiveUniformBlockName(glProgram, index, maxBufferSize, &unusedSize, uniformName);

			GpuParamBlockDesc newBlockDesc;
			newBlockDesc.slot = index + 1;
			newBlockDesc.name = uniformName;
			newBlockDesc.blockSize = 0;

			returnParamDesc.paramBlocks[newBlockDesc.name] = newBlockDesc;
			blockSlotToName.insert(std::make_pair(newBlockDesc.slot, newBlockDesc.name));
		}

		// get the number of active uniforms
		GLint uniformCount = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORMS, &uniformCount);

		// Loop over each of the active uniforms, and add them to the reference container
		// only do this for user defined uniforms, ignore built in gl state uniforms
		for (GLuint index = 0; index < (GLuint)uniformCount; index++)
		{
			GLsizei arraySize = 0;
			glGetActiveUniformName(glProgram, index, maxBufferSize, &arraySize, uniformName);

			String paramName = String(uniformName);

			// If the uniform name has a "[" in it then its an array element uniform.
			String::size_type arrayStart = paramName.find("[");
			if (arrayStart != String::npos)
			{
				// if not the first array element then skip it and continue to the next uniform
				if (paramName.compare(arrayStart, paramName.size() - 1, "[0]") != 0)
					CM_EXCEPT(NotImplementedException, "No support for array parameters yet.");

				paramName = paramName.substr(0, arrayStart);
			}

			GLint uniformType;
			glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_TYPE, &uniformType);

			bool isSampler = false;
			switch(uniformType)
			{
			case GL_SAMPLER_1D:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
				isSampler = true;
			}

			if(isSampler)
			{
				GpuParamSpecialDesc samplerParam;
				samplerParam.name = paramName;
				samplerParam.slot = glGetUniformLocation(glProgram, uniformName);

				GpuParamSpecialDesc textureParam;
				textureParam.name = paramName;
				textureParam.slot = samplerParam.slot;

				switch(uniformType)
				{
				case GL_SAMPLER_1D:
					samplerParam.type = GST_SAMPLER1D;
					textureParam.type = GST_TEXTURE1D;
					break;
				case GL_SAMPLER_2D:
					samplerParam.type = GST_SAMPLER2D;
					textureParam.type = GST_TEXTURE2D;
					break;
				case GL_SAMPLER_3D:
					samplerParam.type = GST_SAMPLER3D;
					textureParam.type = GST_TEXTURE3D;
					break;
				case GL_SAMPLER_CUBE:
					samplerParam.type = GST_SAMPLERCUBE;
					textureParam.type = GST_TEXTURECUBE;
					break;
				}

				returnParamDesc.samplers.insert(std::make_pair(paramName, samplerParam));
				returnParamDesc.textures.insert(std::make_pair(paramName, textureParam));
			}
			else
			{
				GpuParamMemberDesc gpuParam;
				gpuParam.name = paramName;

				GLint blockIndex;
				glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

				determineParamInfo(gpuParam, paramName, glProgram, index);

				if(blockIndex != -1)
				{
					GLint blockOffset;
					glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_OFFSET, &blockOffset);
					gpuParam.gpuMemOffset = blockOffset;

					GLint arrayStride;
					glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
					gpuParam.elementSize = arrayStride;

					gpuParam.paramBlockSlot = blockIndex + 1; // 0 is reserved for globals

					String& blockName = blockSlotToName[gpuParam.paramBlockSlot];
					GpuParamBlockDesc& curBlockDesc = returnParamDesc.paramBlocks[blockName];

					gpuParam.cpuMemOffset = curBlockDesc.blockSize;
					curBlockDesc.blockSize += gpuParam.elementSize * gpuParam.arraySize;
				}
				else
				{
					gpuParam.gpuMemOffset = glGetUniformLocationARB(glProgram, uniformName);
					gpuParam.paramBlockSlot = 0;
					gpuParam.cpuMemOffset = globalBlockDesc.blockSize;

					globalBlockDesc.blockSize += gpuParam.elementSize * gpuParam.arraySize;
				}

				returnParamDesc.params.insert(std::make_pair(gpuParam.name, gpuParam));
			}
		}

		delete[] uniformName;
	}

	void GLSLParamParser::determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex)
	{
		GLint arraySize;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_SIZE, &arraySize);
		desc.arraySize = arraySize;

		GLint uniformType;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_TYPE, &uniformType);

		switch (uniformType)
		{
		case GL_BOOL:
			desc.type = GMT_BOOL;
			desc.elementSize = 1;
			break;
		case GL_FLOAT:
			desc.type = GMT_FLOAT1;
			desc.elementSize = 1;
			break;
		case GL_FLOAT_VEC2:
			desc.type = GMT_FLOAT2;
			desc.elementSize = 2;
			break;
		case GL_FLOAT_VEC3:
			desc.type = GMT_FLOAT3;
			desc.elementSize = 3;
			break;
		case GL_FLOAT_VEC4:
			desc.type = GMT_FLOAT4;
			desc.elementSize = 4;
			break;
		case GL_INT:
			desc.type = GMT_INT1;
			desc.elementSize = 1;
			break;
		case GL_INT_VEC2:
			desc.type = GMT_INT2;
			desc.elementSize = 2;
			break;
		case GL_INT_VEC3:
			desc.type = GMT_INT3;
			desc.elementSize = 3;
			break;
		case GL_INT_VEC4:
			desc.type = GMT_INT4;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT2:
			desc.type = GMT_MATRIX_2X2;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT3:
			desc.type = GMT_MATRIX_3X3;
			desc.elementSize = 9;
			break;
		case GL_FLOAT_MAT4:
			desc.type = GMT_MATRIX_4X4;
			desc.elementSize = 16;
			break;
		case GL_FLOAT_MAT2x3:
			desc.type = GMT_MATRIX_2X3;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT3x2:
			desc.type = GMT_MATRIX_3X2;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT2x4:
			desc.type = GMT_MATRIX_2X4;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT4x2:
			desc.type = GMT_MATRIX_4X2;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT3x4:
			desc.type = GMT_MATRIX_3X4;
			desc.elementSize = 12;
			break;
		case GL_FLOAT_MAT4x3:
			desc.type = GMT_MATRIX_4X3;
			desc.elementSize = 12;
			break;
		default:
			CM_EXCEPT(InternalErrorException, "Invalid shader parameter type: " + toString(uniformType) + " for parameter " + paramName);
		}
	}

    //---------------------------------------------------------------------------
    GLSLProgram::~GLSLProgram()
    {
        unload_internal();
    }
	//-----------------------------------------------------------------------
	GLSLProgram::GLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
		: HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired),
		mInputOperationType(RenderOperation::OT_TRIANGLE_LIST),
		mOutputOperationType(RenderOperation::OT_TRIANGLE_LIST), mMaxOutputVertices(3)
	{
		// Manually assign language now since we use it immediately
		mSyntaxCode = "glsl";

	}
    //-----------------------------------------------------------------------
	void GLSLProgram::loadFromSource(void)
	{
		// only create a shader object if glsl is supported
		GLenum shaderType = 0x0000;
		if (isSupported())
		{
			checkForGLSLError( "GLSLProgram::loadFromSource", "GL Errors before creating shader object", 0, GLSLOT_SHADER);

			switch (mType)
			{
			case GPT_VERTEX_PROGRAM:
				shaderType = GL_VERTEX_SHADER;
				break;
			case GPT_FRAGMENT_PROGRAM:
				shaderType = GL_FRAGMENT_SHADER;
				break;
			case GPT_GEOMETRY_PROGRAM:
				shaderType = GL_GEOMETRY_SHADER;
				break;
			case GPT_HULL_PROGRAM:
				shaderType = GL_TESS_CONTROL_SHADER;
				break;
			case GPT_DOMAIN_PROGRAM:
				shaderType = GL_TESS_EVALUATION_SHADER;
				break;
			}
		}

		// Preprocess the GLSL shader in order to get a clean source
		CPreprocessor cpp;

		// Pass all user-defined macros to preprocessor
		if (!mPreprocessorDefines.empty ())
		{
			String::size_type pos = 0;
			while (pos != String::npos)
			{
				// Find delims
				String::size_type endPos = mPreprocessorDefines.find_first_of(";,=", pos);
				if (endPos != String::npos)
				{
					String::size_type macro_name_start = pos;
					size_t macro_name_len = endPos - pos;
					pos = endPos;

					// Check definition part
					if (mPreprocessorDefines[pos] == '=')
					{
						// set up a definition, skip delim
						++pos;
						String::size_type macro_val_start = pos;
						size_t macro_val_len;

						endPos = mPreprocessorDefines.find_first_of(";,", pos);
						if (endPos == String::npos)
						{
							macro_val_len = mPreprocessorDefines.size () - pos;
							pos = endPos;
						}
						else
						{
							macro_val_len = endPos - pos;
							pos = endPos+1;
						}
						cpp.Define (
							mPreprocessorDefines.c_str () + macro_name_start, macro_name_len,
							mPreprocessorDefines.c_str () + macro_val_start, macro_val_len);
					}
					else
					{
						// No definition part, define as "1"
						++pos;
						cpp.Define (
							mPreprocessorDefines.c_str () + macro_name_start, macro_name_len, 1);
					}
				}
				else
					pos = endPos;
			}
		}

		size_t out_size = 0;
		const char *src = mSource.c_str ();
		size_t src_len = mSource.size ();
		char *out = cpp.Parse (src, src_len, out_size);
		if (!out || !out_size)
		{
			// Failed to preprocess, break out
			CM_EXCEPT(RenderingAPIException, "Failed to preprocess shader ");
		}

		mSource = String (out, out_size);
		if (out < src || out > src + src_len)
			free (out);

		// Add preprocessor extras and main source
		if (!mSource.empty())
		{
			const char *source = mSource.c_str();
			mGLHandle = glCreateShaderProgramv(shaderType, 1, &source);
			// check for load errors
			checkForGLSLError( "GLSLProgram::loadFromSource", "Cannot load GLSL high-level shader source : ", 0, GLSLOT_PROGRAM);
		}

		mAssemblerProgram = GpuProgramPtr(new GLSLGpuProgram(this, mSource, mEntryPoint, mSyntaxCode, mType, mProfile));

		GLSLParamParser paramParser;
		paramParser.buildUniformDescriptions(mGLHandle, mParametersDesc);
	}
	//---------------------------------------------------------------------------
	void GLSLProgram::unload_internal()
	{   
		// We didn't create mAssemblerProgram through a manager, so override this
		// implementation so that we don't try to remove it from one. Since getCreator()
		// is used, it might target a different matching handle!
		mAssemblerProgram = nullptr;

		if (isSupported())
			glDeleteShader(mGLHandle);

		HighLevelGpuProgram::unload_internal();
	}
    //-----------------------------------------------------------------------
    const String& GLSLProgram::getLanguage(void) const
    {
        static const String language = "glsl";

        return language;
    }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* GLSLProgram::getRTTIStatic()
	{
		return GLSLProgramRTTI::instance();
	}

	RTTITypeBase* GLSLProgram::getRTTI() const
	{
		return GLSLProgram::getRTTIStatic();
	}
}
