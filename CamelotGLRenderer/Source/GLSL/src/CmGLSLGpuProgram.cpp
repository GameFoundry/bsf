#pragma once

#include "CmGLSLGpuProgram.h"
#include "CmRenderSystem.h"
#include "CmException.h"
#include "CmGLSLPreprocessor.h"
#include "CmGLSLParamParser.h"
#include "CmHardwareBufferManager.h"
#include "CmGLSLGpuProgramRTTI.h"

namespace BansheeEngine {

	UINT32 GLSLGpuProgram::mVertexShaderCount = 0;
	UINT32 GLSLGpuProgram::mFragmentShaderCount = 0;
	UINT32 GLSLGpuProgram::mGeometryShaderCount = 0;
	UINT32 GLSLGpuProgram::mDomainShaderCount = 0;
	UINT32 GLSLGpuProgram::mHullShaderCount = 0;

	bool checkForGLSLError(const GLuint programObj, String& outErrorMsg)
	{
		StringStream stream;

		GLenum glErr;
		bool errorsFound = false;

		glErr = glGetError();
		while (glErr != GL_NO_ERROR)
		{
			const char* glerrStr = (const char*)gluErrorString(glErr);

			if (glerrStr)
			{
				if (errorsFound)
					stream << "\nPrevious errors: \n";

				stream << String(glerrStr) << std::endl;;
			}

			glErr = glGetError();
			errorsFound = true;
		}

		if (errorsFound && programObj > 0)
		{
			GLint infologLength = 0;
			glGetProgramiv(programObj, GL_INFO_LOG_LENGTH, &infologLength);

			if (infologLength > 0)
			{
				GLint charsWritten = 0;

				GLchar* infoLog = (GLchar*)cm_alloc<ScratchAlloc>(sizeof(GLchar)* infologLength);

				glGetProgramInfoLog(programObj, infologLength, &charsWritten, infoLog);

				stream << "Compile and linker info log: \n";
				stream << String(infoLog);

				cm_free<ScratchAlloc>(infoLog);
			}
		}

		outErrorMsg = stream.str();

		return errorsFound;
	}
	
	GLSLGpuProgram::GLSLGpuProgram(const String& source, const String& entryPoint, GpuProgramType gptype, 
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired)
		:GpuProgram(source, entryPoint, gptype, profile, includes, isAdjacencyInfoRequired),
		mInputOperationType(DOT_TRIANGLE_LIST), mOutputOperationType(DOT_TRIANGLE_LIST), mMaxOutputVertices(3),
		mProgramID(0), mGLHandle(0)
    { }

    GLSLGpuProgram::~GLSLGpuProgram()
    {
    }

	void GLSLGpuProgram::initialize_internal()
	{
		if (!isSupported())
		{
			mIsCompiled = false;
			mCompileError = "Specified program is not supported by the current render system.";

			return;
		}

		mVertexDeclaration = HardwareBufferManager::instance().createVertexDeclaration();

		// only create a shader object if glsl is supported
		GLenum shaderType = 0x0000;
		switch (mType)
		{
		case GPT_VERTEX_PROGRAM:
			shaderType = GL_VERTEX_SHADER;
			mProgramID = ++mVertexShaderCount;
			break;
		case GPT_FRAGMENT_PROGRAM:
			shaderType = GL_FRAGMENT_SHADER;
			mProgramID = ++mFragmentShaderCount;
			break;
		case GPT_GEOMETRY_PROGRAM:
			shaderType = GL_GEOMETRY_SHADER;
			mProgramID = ++mGeometryShaderCount;
			break;
		case GPT_HULL_PROGRAM:
			shaderType = GL_TESS_CONTROL_SHADER;
			mProgramID = ++mDomainShaderCount;
			break;
		case GPT_DOMAIN_PROGRAM:
			shaderType = GL_TESS_EVALUATION_SHADER;
			mProgramID = ++mHullShaderCount;
			break;
		}

		// Preprocess the GLSL shader in order to get a clean source
		CPreprocessor cpp;

		// Pass all user-defined macros to preprocessor
		if (!mPreprocessorDefines.empty())
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
							macro_val_len = mPreprocessorDefines.size() - pos;
							pos = endPos;
						}
						else
						{
							macro_val_len = endPos - pos;
							pos = endPos + 1;
						}
						cpp.Define(
							mPreprocessorDefines.c_str() + macro_name_start, macro_name_len,
							mPreprocessorDefines.c_str() + macro_val_start, macro_val_len);
					}
					else
					{
						// No definition part, define as "1"
						++pos;
						cpp.Define(
							mPreprocessorDefines.c_str() + macro_name_start, macro_name_len, 1);
					}
				}
				else
					pos = endPos;
			}
		}

		size_t out_size = 0;
		const char *src = mSource.c_str();
		size_t src_len = mSource.size();
		char *out = cpp.Parse(src, src_len, out_size);
		if (!out || !out_size)
		{
			// Failed to preprocess, break out
			CM_EXCEPT(RenderingAPIException, "Failed to preprocess shader.");
		}

		mSource = String(out, out_size);
		if (out < src || out > src + src_len)
			free(out);

		// Add preprocessor extras and main source
		if (!mSource.empty())
		{
			const char *source = mSource.c_str();
			mGLHandle = glCreateShaderProgramv(shaderType, 1, &source);
			
			mCompileError = "";
			mIsCompiled = !checkForGLSLError(mGLHandle, mCompileError);
		}

		if (mIsCompiled)
		{
			GLSLParamParser paramParser;
			paramParser.buildUniformDescriptions(mGLHandle, mParametersDesc);
			paramParser.buildVertexDeclaration(mGLHandle, *mVertexDeclaration);
		}

		GpuProgram::initialize_internal();
	}

	void GLSLGpuProgram::destroy_internal()
	{
		if (mIsCompiled && mGLHandle != 0)
		{
			glDeleteProgram(mGLHandle);
			mGLHandle = 0;
		}

		GpuProgram::destroy_internal();
	}

	const String& GLSLGpuProgram::getLanguage() const
	{
		static const String language = "glsl";

		return language;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* GLSLGpuProgram::getRTTIStatic()
	{
		return GLSLGpuProgramRTTI::instance();
	}

	RTTITypeBase* GLSLGpuProgram::getRTTI() const
	{
		return GLSLGpuProgram::getRTTIStatic();
	}
}

