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
#ifndef __GLPrerequisites_H__
#define __GLPrerequisites_H__

#include "CmPrerequisites.h"

namespace CamelotEngine {
    // Forward declarations
    class GLSupport;
    class GLRenderSystem;
    class GLTexture;
    class GLTextureManager;
    class GLGpuProgram;
    class GLContext;
    class GLRTTManager;
    class GLHardwarePixelBuffer;
    class GLRenderBuffer;
	class GLGpuParamBlock;

	typedef std::shared_ptr<GLHardwarePixelBuffer> GLHardwarePixelBufferPtr;
	typedef std::shared_ptr<GLRenderBuffer> GLRenderBufferPtr;
	typedef std::shared_ptr<GLGpuParamBlock> GLGpuParamBlockPtr;

	enum TypeID_D3D9
	{
		TID_GL_GLSLProgram = 11000
	};
}

#if CM_THREAD_SUPPORT == 1
#	define GLEW_MX
#endif

#if CM_PLATFORM == CM_PLATFORM_WIN32
#if !defined( __MINGW32__ )
#   define WIN32_LEAN_AND_MEAN
#  ifndef NOMINMAX
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#endif
#   include <windows.h>
#   include <wingdi.h>
#   include <GL/glew.h>
#   include <GL/wglew.h>
#   include <GL/glu.h>
#elif CM_PLATFORM == CM_PLATFORM_LINUX
#   include <GL/glew.h>
#   include <GL/glu.h>
#   define GL_GLEXT_PROTOTYPES
#elif CM_PLATFORM == CM_PLATFORM_APPLE
#   include <GL/glew.h>
#   include <OpenGL/glu.h>
#endif

#if CM_THREAD_SUPPORT == 1
	// implemented in OgreGLContext.cpp
	GLEWContext * glewGetContext();

#	if CM_PLATFORM == CM_PLATFORM_WIN32
	// implemented in OgreWin32Context.cpp
	WGLEWContext * wglewGetContext();
#	endif

#endif

/// Lots of generated code in here which triggers the new VC CRT security warnings
#if !defined( _CRT_SECURE_NO_DEPRECATE )
#define _CRT_SECURE_NO_DEPRECATE
#endif

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__) && !defined(CM_STATIC_LIB)
#	ifdef CM_RSGL_EXPORTS
#		define CM_RSGL_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define CM_RSGL_EXPORT
#       else
#    		define CM_RSGL_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define CM_RSGL_EXPORT  __attribute__ ((visibility("default")))
#else
#    define CM_RSGL_EXPORT
#endif

#endif //#ifndef __GLPrerequisites_H__
