/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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

#include "CmGLSLExtSupport.h"
#include "CmGLSupport.h"
#include "CmDebug.h"

namespace CamelotFramework
{
    void checkForGLSLError(const String& ogreMethod, const String& errorTextPrefix, const GLuint obj, 
		GLSLObjectType objectType, const bool forceInfoLog, const bool forceException)
    {
		GLenum glErr;
		bool errorsFound = false;
		String msg = errorTextPrefix;

		// get all the GL errors
		glErr = glGetError();
		while (glErr != GL_NO_ERROR)
        {
			const char* glerrStr = (const char*)gluErrorString(glErr);
			if (glerrStr)
			{
				msg += String(glerrStr);
			}
			glErr = glGetError();
			errorsFound = true;
        }


		// if errors were found then put them in the Log and raise and exception
		if (errorsFound || forceInfoLog)
		{
			// if shader or program object then get the log message and send to the log manager
			bool infoLogMessageExists = false;
			if(objectType == GLSLOT_SHADER)
				infoLogMessageExists = logShaderInfo(msg, obj);
			else if(objectType == GLSLOT_PROGRAM)
				infoLogMessageExists = logProgramInfo(msg, obj);

            if (forceException) 
			{
				CM_EXCEPT(InternalErrorException, msg);
			}
			else
			{
				if(errorsFound)
				{
					LOGWRN(msg);
				}
				else if(infoLogMessageExists)
				{
					LOGINFO(msg);
				}
			}
		}
    }

	bool logShaderInfo(String& msg, const GLuint obj)
	{
		String logMessage = msg;

		if (obj > 0)
		{
			GLint infologLength = 0;

			glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

			if (infologLength > 0)
			{
				GLint charsWritten  = 0;

				GLchar* infoLog = (GLchar*)cm_alloc<ScratchAlloc>(sizeof(GLchar) * infologLength);

				glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
				logMessage += String(infoLog);

				cm_free<ScratchAlloc>(infoLog);

				if(charsWritten > 0)
					return true;
			}
		}

		return false;
	}

	bool logProgramInfo(String& msg, const GLuint obj)
	{
		if (obj > 0)
		{
			GLint infologLength = 0;

			glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

			if (infologLength > 0)
			{
				GLint charsWritten  = 0;

				GLchar* infoLog = (GLchar*)cm_alloc<ScratchAlloc>(sizeof(GLchar) * infologLength);

				glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
				msg += String(infoLog);

				cm_free<ScratchAlloc>(infoLog);

				if(charsWritten > 0)
					return true;
			}
		}

		return false;
	}

} // namespace CamelotFramework
