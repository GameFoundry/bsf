/*-------------------------------------------------------------------------
This source file is a part of OGRE
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
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef __OgrePrerequisites_H__
#define __OgrePrerequisites_H__

#include "CmPrerequisitesUtil.h"

#define CM_MAX_TEXTURE_LAYERS 16
#define CM_MAX_MULTIPLE_RENDER_TARGETS 8

//----------------------------------------------------------------------------
// Windows Settings
#if CM_PLATFORM == CM_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( CM_STATIC_LIB )
// Linux compilers don't have symbol import/export directives.
#   	define CM_EXPORT
#   else
#   	if defined( CM_EXPORTS )
#       	define CM_EXPORT __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define CM_EXPORT
#           else
#       	    define CM_EXPORT __declspec( dllimport )
#           endif
#   	endif
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define CM_DEBUG_MODE 1
#   else
#       define CM_DEBUG_MODE 0
#   endif

#endif // CM_PLATFORM == CM_PLATFORM_WIN32

namespace CamelotEngine {

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
    class Camera;
    class Color;
    class GpuProgram;
    class GpuProgramManager;
	class GpuProgramParameters;
    class HardwareIndexBuffer;
    class HardwareOcclusionQuery;
    class HardwareVertexBuffer;
	class HardwarePixelBuffer;
	class HighLevelGpuProgram;
	class HighLevelGpuProgramManager;
	class HighLevelGpuProgramFactory;
    class IndexData;
    class Pass;
	class Technique;
	class Shader;
	class Material;
    class RenderSystem;
    class RenderSystemCapabilities;
    class RenderTarget;
    class RenderTexture;
	class MultiRenderTarget;
    class RenderWindow;
    class RenderOperation;
    class StringInterface;
    class SamplerState;
    class TextureManager;
    class Viewport;
    class VertexBufferBinding;
    class VertexData;
    class VertexDeclaration;
	class Input;
	class InputHandler;
	class Renderable;
	class Renderer;
	class RendererFactory;
	class RenderCommandBuffer;
	class WorkQueue;
	class PassParameters;
	class AsyncOp;
	class HardwareBufferManagerBase;
	class HardwareConstantBuffer;
	class CommandQueue;
	class DeferredRenderContext;
	class DepthStencilState;
	class RenderStateManager;
	class RasterizerState;
	class BlendState;
	// Asset import
	class SpecificImporter;
	class Importer;
	// Resources
	class Resource;
	class Resources;
	class Texture;
	class Mesh;
	// Scene
	class GameObject;
	class Component;
	class SceneManager;
	// RTTI
	class MeshRTTI;
	// Desc structs
	struct SAMPLER_STATE_DESC;
	struct DEPTH_STENCIL_STATE_DESC;
	struct RASTERIZER_STATE_DESC;
	struct BLEND_STATE_DESC;
	struct RENDER_TARGET_BLEND_STATE_DESC;
}

/* Shared pointer typedefs*/
namespace CamelotEngine
{
	typedef std::shared_ptr<GpuProgram> GpuProgramPtr;
	typedef std::shared_ptr<GpuProgramParameters> GpuProgramParametersPtr;
	typedef std::shared_ptr<HighLevelGpuProgram> HighLevelGpuProgramPtr;
	typedef std::shared_ptr<HardwarePixelBuffer> HardwarePixelBufferPtr;
	typedef std::shared_ptr<VertexDeclaration> VertexDeclarationPtr;
	typedef std::shared_ptr<Mesh> MeshPtr;
	typedef std::shared_ptr<Texture> TexturePtr;
	typedef std::shared_ptr<Resource> ResourcePtr;
	typedef std::shared_ptr<Camera> CameraPtr;
	typedef std::shared_ptr<Renderable> RenderablePtr;
	typedef std::shared_ptr<Technique> TechniquePtr;
	typedef std::shared_ptr<Pass> PassPtr;
	typedef std::shared_ptr<Shader> ShaderPtr;
	typedef std::shared_ptr<Material> MaterialPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	typedef std::shared_ptr<RendererFactory> RendererFactoryPtr;
	typedef std::shared_ptr<WorkQueue> WorkQueuePtr;
	typedef std::shared_ptr<PassParameters> PassParametersPtr;
	typedef std::shared_ptr<Component> ComponentPtr;
	typedef std::shared_ptr<GameObject> GameObjectPtr;
	typedef std::shared_ptr<HardwareConstantBuffer> HardwareConstantBufferPtr;
	typedef std::shared_ptr<DeferredRenderContext> DeferredRenderContextPtr;
	typedef std::shared_ptr<SamplerState> SamplerStatePtr;
	typedef std::shared_ptr<DepthStencilState> DepthStencilStatePtr;
	typedef std::shared_ptr<RasterizerState> RasterizerStatePtr;
	typedef std::shared_ptr<BlendState> BlendStatePtr;
}

// All type IDs
namespace CamelotEngine
{
	enum TypeID_Core
	{
		TID_Texture = 1001,
		TID_Mesh = 1002,
		TID_MeshData = 1003,
		TID_VertexDeclaration = 1004,
		TID_VertexData = 1005,
		TID_Component = 1006,
		TID_Camera = 1007,
		TID_Renderable = 1008,
		TID_ResourceHandle = 1009,
		TID_GpuProgram = 1010,
		TID_ResourceHandleData = 1011,
		TID_CgProgram = 1012,
		TID_ResourceMetaData = 1013,
		TID_Pass = 1014,
		TID_Technique = 1015,
		TID_Shader = 1016,
		TID_Material = 1017,
		TID_MaterialParams = 1018,
		TID_FloatParamKVP = 1019,
		TID_MaterialTexParamKVP = 1020,
		TID_SamplerState = 1021,
		TID_SamplerStateParamKVP = 1022,
		TID_BlendState = 1023,
		TID_RasterizerState = 1024,
		TID_DepthStencilState = 1025
	};
}

/************************************************************************/
/* 							Resource references                   		*/
/************************************************************************/

#include "CmResourceHandle.h"

namespace CamelotEngine
{
	typedef ResourceHandle<Resource> BaseResourceHandle;
	typedef ResourceHandle<Texture> TextureHandle;
	typedef ResourceHandle<Mesh> MeshHandle;
	typedef ResourceHandle<GpuProgram> GpuProgramHandle;
	typedef ResourceHandle<HighLevelGpuProgram> HighLevelGpuProgramHandle;
	typedef ResourceHandle<Material> MaterialHandle;
}

#endif // __OgrePrerequisites_H__


