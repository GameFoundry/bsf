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
#ifndef __D3D9PREREQUISITES_H__
#define __D3D9PREREQUISITES_H__

#include "CmPrerequisites.h"

#define D3D9_DEVICE_ACCESS_LOCK	
#define D3D9_DEVICE_ACCESS_UNLOCK
#define D3D9_DEVICE_ACCESS_CRITICAL_SECTION

// Define versions for if DirectX is in use (Win32 only)
#define DIRECT3D_VERSION 0x0900

// some D3D commonly used macros
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

// enable extended d3d debug 
#if CM_DEBUG_MODE
#	define D3D_DEBUG_INFO
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>


namespace BansheeEngine
{
	// Predefine classes
	class D3D9RenderSystem;
	class D3D9RenderWindow;
	class D3D9Texture;
	class D3D9TextureManager;
	class D3D9Driver;
	class D3D9DriverList;
	class D3D9VideoMode;
	class D3D9VideoModeList;
	class D3D9Device;
	class D3D9DeviceManager;
	class D3D9ResourceManager;
	class D3D9GpuProgram;
	class D3D9GpuProgramManager;
    class D3D9HardwareBufferManager;
    class D3D9IndexBuffer;
    class D3D9HLSLProgramFactory;
    class D3D9HLSLProgram;
    class D3D9VertexDeclaration;
	class D3D9Resource;

	typedef std::shared_ptr<D3D9RenderWindow> D3D9RenderWindowPtr;
	typedef std::shared_ptr<D3D9Texture> D3D9TexturePtr;

	enum TypeID_D3D9
	{
		TID_D3D9_HLSLProgram = 10000
	};

    //-------------------------------------------
	// Windows setttings
	//-------------------------------------------
#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(CM_STATIC_LIB)
#	ifdef CM_RSD3D9_EXPORTS
#		define CM_D3D9_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define CM_D3D9_EXPORT
#       else
#    		define CM_D3D9_EXPORT __declspec(dllimport)
#       endif
#	endif
#else
#	define CM_D3D9_EXPORT
#endif	// CM_PLATFORM_WIN32
}
#endif
