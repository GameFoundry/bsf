#pragma once

#include "BsCorePrerequisites.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#if BS_DEBUG_MODE
#	define D3D_DEBUG_INFO
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif

#include <d3d11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>

namespace BansheeEngine
{
	class D3D11RenderSystem;
	class D3D11RenderWindow;
	class D3D11Texture;
	class D3D11TextureManager;
	class D3D11DepthBuffer;
	class D3D11Driver;
	class D3D11DriverList;
	class D3D11VideoMode;
	class D3D11VideoModeList;
	class D3D11GpuProgramManager;
	class D3D11IndexBuffer;
	class D3D11HardwareConstantBuffer;
	class D3D11HLSLProgramFactory;
	class D3D11HLSLProgram;
	class D3D11VertexDeclaration;
	class D3D11Device;
	class D3D11HardwareBuffer;
	class D3D11GpuVertexProgram;
	class D3D11GpuFragmentProgram;
	class D3D11GpuGeometryProgram;
	class D3D11GpuHullProgram;
	class D3D11GpuDomainProgram;
	class D3D11BlendState;
	class D3D11RasterizerState;
	class D3D11SamplerState;
	class D3D11DepthStencilState;
	class D3D11InputLayoutManager;
	class D3D11GpuBufferCore;
	class D3D11RenderUtility;
	class D3D11GpuProgramCore;

	/**
	 * @brief	DirectX 11 specific types to track resource statistics for.
	 */
	enum D3D11RenderStatResourceType
	{
		RenderStatObject_DepthStencilState = 100,
		RenderStatObject_RasterizerState,
		RenderStatObject_BlendState,
		RenderStatObject_SamplerState,
		RenderStatObject_InputLayout,
		RenderStatObject_ResourceView,
		RenderStatObject_SwapChain
	};

	typedef Vector<char*> HLSLMicroCode;

	typedef std::shared_ptr<D3D11GpuVertexProgram> D3D11GpuVertexProgramPtr;
	typedef std::shared_ptr<D3D11GpuFragmentProgram> D3D11GpuFragmentProgramPtr;
	typedef std::shared_ptr<D3D11GpuGeometryProgram> D3D11GpuGeometryProgramPtr;
	typedef std::shared_ptr<D3D11GpuHullProgram> D3D11GpuHullProgramPtr;
	typedef std::shared_ptr<D3D11GpuDomainProgram> D3D11GpuDomainProgramPtr;
	typedef std::shared_ptr<D3D11BlendState> D3D11BlendStatePtr;
	typedef std::shared_ptr<D3D11RasterizerState> D3D11RasterizerStatePtr;
	typedef std::shared_ptr<D3D11SamplerState> D3D11SamplerStatePtr;
	typedef std::shared_ptr<D3D11DepthStencilState> D3D11DepthStencilStatePtr;
	typedef std::shared_ptr<D3D11RenderWindow> D3D11RenderWindowPtr;

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(BS_STATIC_LIB)
#	ifdef BS_RSD3D11_EXPORTS
#		define BS_D3D11_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_D3D11_EXPORT
#       else
#    		define BS_D3D11_EXPORT __declspec(dllimport)
#       endif
#	endif
#else
#	define BS_D3D11_EXPORT
#endif
}