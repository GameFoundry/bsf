#pragma once

#include "BsCorePrerequisites.h"

#define D3D9_DEVICE_ACCESS_LOCK	
#define D3D9_DEVICE_ACCESS_UNLOCK
#define D3D9_DEVICE_ACCESS_CRITICAL_SECTION

#define DIRECT3D_VERSION 0x0900

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#if BS_DEBUG_MODE
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
	class D3D9RenderWindowCore;
	class D3D9TextureCore;
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
    class D3D9IndexBuffer;
    class D3D9HLSLProgramFactory;
    class D3D9HLSLProgramCore;
    class D3D9VertexDeclarationCore;
	class D3D9Resource;

	typedef std::shared_ptr<D3D9RenderWindow> D3D9RenderWindowPtr;

	/**
	 * @brief	DirectX 9 specific types to track resource statistics for.
	 */
	enum D3D9RenderStatResourceType
	{
		RenderStatObject_VertexDeclaration = 100
	};

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(BS_STATIC_LIB)
#	ifdef BS_RSD3D9_EXPORTS
#		define BS_D3D9_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_D3D9_EXPORT
#       else
#    		define BS_D3D9_EXPORT __declspec(dllimport)
#       endif
#	endif
#else
#	define BS_D3D9_EXPORT
#endif
}