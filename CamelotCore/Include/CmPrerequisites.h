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

#define CM_MAX_MULTIPLE_RENDER_TARGETS 8
#define CM_FORCE_SINGLETHREADED_RENDERING 0

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

// Linux/Apple Settings
#if CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( CM_GCC_VISIBILITY )
#       define CM_EXPORT  __attribute__ ((visibility("default")))
#       define CM_HIDDEN __attribute__ ((visibility("hidden")))
#   else
#       define CM_EXPORT
#       define CM_HIDDEN
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

#   ifdef DEBUG
#       define CM_DEBUG_MODE 1
#   else
#       define CM_DEBUG_MODE 0
#   endif

#endif

#include "CmMemAllocCategories.h"

namespace CamelotFramework {

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
    class Color;
    class GpuProgram;
    class GpuProgramManager;
    class IndexBuffer;
    class OcclusionQuery;
    class VertexBuffer;
	class PixelBuffer;
	class GpuBuffer;
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
	class MultiRenderTexture;
    class RenderWindow;
    class RenderOperation;
    class StringInterface;
    class SamplerState;
    class TextureManager;
    class Viewport;
    class VertexData;
    class VertexDeclaration;
	class Input;
	class InputHandler;
	class Renderer;
	class RendererFactory;
	class RenderCommandBuffer;
	class WorkQueue;
	class PassParameters;
	class AsyncOp;
	class HardwareBufferManager;
	class FontManager;
	class CommandQueue;
	class DeferredRenderContext;
	class DepthStencilState;
	class RenderStateManager;
	class RasterizerState;
	class BlendState;
	class GpuParamBlock;
	class GpuParamBlockBuffer;
	class GpuParams;
	struct GpuParamDesc;
	struct GpuParamDataDesc;
	struct GpuParamObjectDesc;
	struct GpuParamBlockDesc;
	class GpuProgInclude;
	class TextureView;
	class CoreObject;
	class ImportOptions;
	struct FontData;
	class GameObject;
	class GpuResource;
	class GpuResourceData;
	class BindableGpuParams;
	// Asset import
	class SpecificImporter;
	class Importer;
	// Resources
	class Resource;
	class Resources;
	class Texture;
	class Mesh;
	class Font;
	// Scene
	class SceneObject;
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
	struct RENDER_TEXTURE_DESC;
	struct RENDER_WINDOW_DESC;
	struct FONT_DESC;
}

/************************************************************************/
/* 						Shared pointer typedefs	                    	*/
/************************************************************************/

namespace CamelotFramework
{
	typedef std::shared_ptr<GpuProgram> GpuProgramPtr;
	typedef std::shared_ptr<HighLevelGpuProgram> HighLevelGpuProgramPtr;
	typedef std::shared_ptr<PixelBuffer> PixelBufferPtr;
	typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
	typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
	typedef std::shared_ptr<GpuBuffer> GpuBufferPtr;
	typedef std::shared_ptr<VertexDeclaration> VertexDeclarationPtr;
	typedef std::shared_ptr<Mesh> MeshPtr;
	typedef std::shared_ptr<Texture> TexturePtr;
	typedef std::shared_ptr<Resource> ResourcePtr;
	typedef std::shared_ptr<Technique> TechniquePtr;
	typedef std::shared_ptr<Pass> PassPtr;
	typedef std::shared_ptr<Shader> ShaderPtr;
	typedef std::shared_ptr<Material> MaterialPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	typedef std::shared_ptr<RendererFactory> RendererFactoryPtr;
	typedef std::shared_ptr<WorkQueue> WorkQueuePtr;
	typedef std::shared_ptr<PassParameters> PassParametersPtr;
	typedef std::shared_ptr<Component> ComponentPtr;
	typedef std::shared_ptr<SceneObject> GameObjectPtr;
	typedef std::shared_ptr<DeferredRenderContext> DeferredRenderContextPtr;
	typedef std::shared_ptr<SamplerState> SamplerStatePtr;
	typedef std::shared_ptr<DepthStencilState> DepthStencilStatePtr;
	typedef std::shared_ptr<RasterizerState> RasterizerStatePtr;
	typedef std::shared_ptr<BlendState> BlendStatePtr;
	typedef std::shared_ptr<RenderWindow> RenderWindowPtr;
	typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
	typedef std::shared_ptr<RenderTexture> RenderTexturePtr;
	typedef std::shared_ptr<MultiRenderTexture> MultiRenderTexturePtr;
	typedef std::shared_ptr<GpuParamBlock> GpuParamBlockPtr;
	typedef std::shared_ptr<GpuParamBlockBuffer> GpuParamBlockBufferPtr;
	typedef std::shared_ptr<GpuParams> GpuParamsPtr;
	typedef std::shared_ptr<TextureView> TextureViewPtr;
	typedef std::shared_ptr<Viewport> ViewportPtr;
	typedef std::shared_ptr<GpuProgInclude> GpuProgIncludePtr;
	typedef std::shared_ptr<ImportOptions> ImportOptionsPtr;
	typedef std::shared_ptr<const ImportOptions> ConstImportOptionsPtr;
	typedef std::shared_ptr<Font> FontPtr;
	typedef std::shared_ptr<InputHandler> InputHandlerPtr;
	typedef std::shared_ptr<GpuResource> GpuResourcePtr;
}

/************************************************************************/
/* 									RTTI                      			*/
/************************************************************************/
namespace CamelotFramework
{
	enum TypeID_Core
	{
		TID_Texture = 1001,
		TID_Mesh = 1002,
		TID_MeshData = 1003,
		TID_VertexDeclaration = 1004,
		TID_VertexElementData = 1005,
		TID_Component = 1006,
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
		TID_DepthStencilState = 1025,
		TID_MaterialParamFloat = 1026,
		TID_MaterialParamVec2 = 1027,
		TID_MaterialParamVec3 = 1028,
		TID_MaterialParamVec4 = 1029,
		TID_MaterialParamMat3 = 1030,
		TID_MaterialParamMat4 = 1031,
		TID_MaterialParamTexture = 1032,
		TID_MaterialParamSamplerState = 1033,
		TID_BLEND_STATE_DESC = 1034,
		TID_SHADER_DATA_PARAM_DESC = 1035,
		TID_SHADER_OBJECT_PARAM_DESC = 1036,
		TID_SHADER_PARAM_BLOCK_DESC = 1047,
		TID_ImportOptions = 1048,
		TID_GpuProgramImportOptions = 1049,
		TID_MaterialParamStruct = 1050,
		TID_Font = 1051,
		TID_FONT_DESC = 1052,
		TID_CHAR_DESC = 1053,
		TID_STDVECTOR = 1054,
		TID_STDMAP = 1055,
		TID_FontImportOptions = 1056,
		TID_FontData = 1057,
		TID_IndexElementData = 1058,
		TID_SceneObject = 1059,
		TID_GameObject = 1060,
		TID_GpuResource = 1061,
		TID_PixelData = 1062
	};

	/**
	 * @brief	Returns true if the provided object can be safely cast into type T.
	 */
	template<class T>
	bool rtti_is_of_type(IReflectable* object)
	{
		BOOST_STATIC_ASSERT_MSG((boost::is_base_of<CamelotFramework::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from CamelotFramework::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}

	/**
	 * @brief	Returns true if the provided object can be safely cast into type T.
	 */
	template<class T>
	bool rtti_is_of_type(std::shared_ptr<IReflectable> object)
	{
		BOOST_STATIC_ASSERT_MSG((boost::is_base_of<CamelotFramework::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from CamelotFramework::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}
}

/************************************************************************/
/* 							Resource references                   		*/
/************************************************************************/

#include "CmResourceHandle.h"

namespace CamelotFramework
{
	// Resource handles
	typedef ResourceHandle<Resource> HResource;
	typedef ResourceHandle<Texture> HTexture;
	typedef ResourceHandle<Mesh> HMesh;
	typedef ResourceHandle<GpuProgram> HGpuProgram;
	typedef ResourceHandle<HighLevelGpuProgram> HHighLevelGpuProgram;
	typedef ResourceHandle<Material> HMaterial;
	typedef ResourceHandle<SamplerState> HSamplerState;
	typedef ResourceHandle<RasterizerState> HRasterizerState;
	typedef ResourceHandle<DepthStencilState> HDepthStencilState;
	typedef ResourceHandle<BlendState> HBlendState;
	typedef ResourceHandle<GpuProgInclude> HGpuProgInclude;
	typedef ResourceHandle<Font> HFont;
}

#endif


