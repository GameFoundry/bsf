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
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmDebug.h"
#include "CmGLSLProgram.h"
#include "CmGLSLGpuProgram.h"
#include "CmGLSLExtSupport.h"
#include "CmGLSLPreprocessor.h"
#include "CmGLSLParamParser.h"

#include "CmGLSLProgramRTTI.h"

namespace CamelotEngine 
{
	//-----------------------------------------------------------------------
	GLSLProgram::GLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
		: HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired),
		mInputOperationType(DOT_TRIANGLE_LIST),
		mOutputOperationType(DOT_TRIANGLE_LIST), mMaxOutputVertices(3)
	{
		// Manually assign language now since we use it immediately
		mSyntaxCode = "glsl";

	}
	//---------------------------------------------------------------------------
	GLSLProgram::~GLSLProgram()
	{

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
			checkForGLSLError( "GLSLProgram::loadFromSource", "Cannot load GLSL high-level shader source : ", mGLHandle, GLSLOT_PROGRAM, true);
		}

		mAssemblerProgram = GpuProgramPtr(new GLSLGpuProgram(this, mSource, mEntryPoint, mSyntaxCode, mType, mProfile));

		GLSLParamParser paramParser;
		paramParser.buildUniformDescriptions(mGLHandle, mParametersDesc);
		paramParser.buildVertexDeclaration(mGLHandle, mVertexDeclaration);
	}
	//---------------------------------------------------------------------------
	void GLSLProgram::destroy_internal()
	{   
		// We didn't create mAssemblerProgram through a manager, so override this
		// implementation so that we don't try to remove it from one. Since getCreator()
		// is used, it might target a different matching handle!
		mAssemblerProgram = nullptr;

		if (isSupported())
			glDeleteShader(mGLHandle);

		HighLevelGpuProgram::destroy_internal();
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
