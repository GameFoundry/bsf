//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsGLSLGpuProgram.h"
#include "BsRenderSystem.h"
#include "BsException.h"
#include "BsGLSLParamParser.h"
#include "BsHardwareBufferManager.h"
#include "BsGLSLGpuProgramRTTI.h"
#include "BsRenderStats.h"

namespace BansheeEngine 
{
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

				GLchar* infoLog = (GLchar*)bs_alloc<ScratchAlloc>(sizeof(GLchar)* infologLength);

				glGetProgramInfoLog(programObj, infologLength, &charsWritten, infoLog);

				stream << "Compile and linker info log: \n";
				stream << String(infoLog);

				bs_free<ScratchAlloc>(infoLog);
			}
		}

		outErrorMsg = stream.str();

		return errorsFound;
	}
	
	GLSLGpuProgram::GLSLGpuProgram(const String& source, const String& entryPoint, GpuProgramType gptype, 
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired)
		:GpuProgram(source, entryPoint, gptype, profile, includes, isAdjacencyInfoRequired),
		mProgramID(0), mGLHandle(0)
    { }

    GLSLGpuProgram::~GLSLGpuProgram()
    { }

	void GLSLGpuProgram::initialize_internal()
	{
		if (!isSupported())
		{
			mIsCompiled = false;
			mCompileError = "Specified program is not supported by the current render system.";

			GpuProgram::initialize_internal();
			return;
		}

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
			paramParser.buildUniformDescriptions(mGLHandle, *mParametersDesc);

			if (mType == GPT_VERTEX_PROGRAM)
			{
				VertexDeclaration::VertexElementList elementList = paramParser.buildVertexDeclaration(mGLHandle);
				mVertexDeclaration = HardwareBufferManager::instance().createVertexDeclaration(elementList);
			}
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuProgram);
		GpuProgram::initialize_internal();
	}

	void GLSLGpuProgram::destroy_internal()
	{
		if (mIsCompiled && mGLHandle != 0)
		{
			glDeleteProgram(mGLHandle);
			mGLHandle = 0;
		}

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuProgram);
		GpuProgram::destroy_internal();
	}

	bool GLSLGpuProgram::isSupported() const
	{
		if (!isRequiredCapabilitiesSupported())
			return false;

		RenderSystem* rs = BansheeEngine::RenderSystem::instancePtr();
		return rs->getCapabilities()->isShaderProfileSupported("glsl");
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

