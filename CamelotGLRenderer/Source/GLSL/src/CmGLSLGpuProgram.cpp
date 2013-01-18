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

#include "CmException.h"
#include "CmGLSLExtSupport.h"
#include "CmGLSLGpuProgram.h"
#include "CmGLSLProgram.h"

namespace CamelotEngine {

	UINT32 GLSLGpuProgram::mVertexShaderCount = 0;
	UINT32 GLSLGpuProgram::mFragmentShaderCount = 0;
	UINT32 GLSLGpuProgram::mGeometryShaderCount = 0;
	UINT32 GLSLGpuProgram::mDomainShaderCount = 0;
	UINT32 GLSLGpuProgram::mHullShaderCount = 0;
    //-----------------------------------------------------------------------------
	GLSLGpuProgram::GLSLGpuProgram(GLSLProgram* parent, const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile) 
		:GpuProgram(source, entryPoint, language, gptype, profile), mGLSLProgram(parent)
    {
        mType = parent->getType();
        mSyntaxCode = "glsl";

		switch(parent->getType())
		{
		case GPT_VERTEX_PROGRAM:
			mProgramID = ++mVertexShaderCount;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mProgramID = ++mFragmentShaderCount;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mProgramID = ++mGeometryShaderCount;
			break;
		case GPT_DOMAIN_PROGRAM:
			mProgramID = ++mDomainShaderCount;
			break;
		case GPT_HULL_PROGRAM:
			mProgramID = ++mHullShaderCount;
			break;
		default:
			CM_EXCEPT(InternalErrorException, "Invalid gpu program type: " + toString(parent->getType()));
		}
    }
    //-----------------------------------------------------------------------
    GLSLGpuProgram::~GLSLGpuProgram()
    {
    }
	//-----------------------------------------------------------------------------
    void GLSLGpuProgram::initialize_internal(void)
    {
		// Nothing to load
		GpuProgram::initialize_internal();
    }
	//-----------------------------------------------------------------------------
	void GLSLGpuProgram::destroy_internal()
	{
		// Nothing to destroy
		GpuProgram::destroy();
	}
}

