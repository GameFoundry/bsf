#pragma once

#include "BsPrerequisitesUtil.h"

#define BS_MAX_MULTIPLE_RENDER_TARGETS 8
#define BS_FORCE_SINGLETHREADED_RENDERING 0

// Windows Settings
#if BS_PLATFORM == BS_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined(BS_STATIC_LIB)
// Linux compilers don't have symbol import/export directives.
#   	define BS_CORE_EXPORT
#   else
#   	if defined(BS_CORE_EXPORTS)
#       	define BS_CORE_EXPORT __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define BS_CORE_EXPORT
#           else
#       	    define BS_CORE_EXPORT __declspec( dllimport )
#           endif
#   	endif
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#endif

// Linux/Apple Settings
#if BS_PLATFORM == BS_PLATFORM_LINUX || BS_PLATFORM == BS_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( BS_GCC_VISIBILITY )
#       define BS_CORE_EXPORT  __attribute__ ((visibility("default")))
#       define BS_HIDDEN __attribute__ ((visibility("hidden")))
#   else
#       define BS_CORE_EXPORT
#       define BS_HIDDEN
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

#   ifdef DEBUG
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#endif

namespace BansheeEngine 
{
    class Color;
    class GpuProgram;
    class GpuProgramManager;
    class IndexBuffer;
    class OcclusionQuery;
    class VertexBuffer;
	class PixelBuffer;
	class GpuBuffer;
	class HighLevelGpuProgram;
	class GpuProgramManager;
	class GpuProgramFactory;
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
    struct RenderOpMesh;
    class StringInterface;
    class SamplerState;
    class TextureManager;
    class Viewport;
    class VertexData;
    class VertexDeclaration;
	class Input;
	struct PointerEvent;
	class RawInputHandler;
	class Renderer;
	class RendererFactory;
	class PassParameters;
	class AsyncOp;
	class HardwareBufferManager;
	class FontManager;
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
	struct RenderOperation;
	class RenderQueue;
	struct ProfilerReport;
	class VertexDataDesc;
	class EventQuery;
	class TimerQuery;
	class OcclusionQuery;
	class FrameAlloc;
	class FolderMonitor;
	class VideoMode;
	class VideoOutputInfo;
	class VideoModeInfo;
	class RenderableElement;
	class CameraProxy;
	struct MaterialProxy;
	struct MaterialProxyPass;
	struct MeshProxy;
	struct ShaderProxy;
	class DrawList;
	// Asset import
	class SpecificImporter;
	class Importer;
	// Resources
	class Resource;
	class Resources;
	class ResourceManifest;
	class Texture;
	class Mesh;
	class MeshBase;
	class TransientMesh;
	class MeshHeap;
	class Font;
	class ResourceMetaData;
	class OSDropTarget;
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

	template<class T>
	class CoreThreadAccessor;
	class CommandQueueNoSync;
	class CommandQueueSync;
}

/************************************************************************/
/* 						Shared pointer typedefs	                    	*/
/************************************************************************/

namespace BansheeEngine
{
	typedef std::shared_ptr<RenderSystem> RenderSystemPtr;
	typedef std::shared_ptr<GpuProgram> GpuProgramPtr;
	typedef std::shared_ptr<PixelBuffer> PixelBufferPtr;
	typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
	typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
	typedef std::shared_ptr<GpuBuffer> GpuBufferPtr;
	typedef std::shared_ptr<VertexDeclaration> VertexDeclarationPtr;
	typedef std::shared_ptr<Mesh> MeshPtr;
	typedef std::shared_ptr<MeshBase> MeshBasePtr;
	typedef std::shared_ptr<MeshHeap> MeshHeapPtr;
	typedef std::shared_ptr<TransientMesh> TransientMeshPtr;
	typedef std::shared_ptr<Texture> TexturePtr;
	typedef std::shared_ptr<Resource> ResourcePtr;
	typedef std::shared_ptr<Technique> TechniquePtr;
	typedef std::shared_ptr<Pass> PassPtr;
	typedef std::shared_ptr<Shader> ShaderPtr;
	typedef std::shared_ptr<Material> MaterialPtr;
	typedef std::shared_ptr<Renderer> RendererPtr;
	typedef std::shared_ptr<RendererFactory> RendererFactoryPtr;
	typedef std::shared_ptr<PassParameters> PassParametersPtr;
	typedef std::shared_ptr<Component> ComponentPtr;
	typedef std::shared_ptr<SceneObject> GameObjectPtr;
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
	typedef std::shared_ptr<GpuResource> GpuResourcePtr;
	typedef std::shared_ptr<VertexDataDesc> VertexDataDescPtr;
	typedef CoreThreadAccessor<CommandQueueNoSync> CoreAccessor;
	typedef CoreThreadAccessor<CommandQueueSync> SyncedCoreAccessor;
	typedef std::shared_ptr<CoreThreadAccessor<CommandQueueNoSync>> CoreAccessorPtr;
	typedef std::shared_ptr<CoreThreadAccessor<CommandQueueSync>> SyncedCoreAccessorPtr;
	typedef std::shared_ptr<EventQuery> EventQueryPtr;
	typedef std::shared_ptr<TimerQuery> TimerQueryPtr;
	typedef std::shared_ptr<OcclusionQuery> OcclusionQueryPtr;
	typedef std::shared_ptr<ResourceManifest> ResourceManifestPtr;
	typedef std::shared_ptr<VideoModeInfo> VideoModeInfoPtr;
	typedef std::shared_ptr<DrawList> DrawListPtr;
	typedef std::shared_ptr<RenderQueue> RenderQueuePtr;
	typedef std::shared_ptr<CameraProxy> CameraProxyPtr;
	typedef std::shared_ptr<MaterialProxy> MaterialProxyPtr;
	typedef std::shared_ptr<MeshProxy> MeshProxyPtr;
	typedef std::shared_ptr<ShaderProxy> ShaderProxyPtr;
	typedef std::shared_ptr<GpuParamDesc> GpuParamDescPtr;
	typedef std::shared_ptr<ResourceMetaData> ResourceMetaDataPtr;
}

/************************************************************************/
/* 									RTTI                      			*/
/************************************************************************/
namespace BansheeEngine
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
		TID_FontImportOptions = 1056,
		TID_FontData = 1057,
		TID_SceneObject = 1059,
		TID_GameObject = 1060,
		TID_GpuResource = 1061,
		TID_PixelData = 1062,
		TID_GpuResourceData = 1063,
		TID_VertexDataDesc = 1064,
		TID_MeshBase = 1065,
		TID_GameObjectHandleBase = 1066,
		TID_ResourceManifest = 1067,
		TID_ResourceManifestEntry = 1068,
		TID_EmulatedParamBlock = 1069,
		TID_TextureImportOptions = 1070,
		TID_ResourceMetaData = 1071
	};
}

/************************************************************************/
/* 							Resource references                   		*/
/************************************************************************/

#include "BsResourceHandle.h"

namespace BansheeEngine
{
	// Resource handles
	typedef ResourceHandle<Resource> HResource;
	typedef ResourceHandle<Texture> HTexture;
	typedef ResourceHandle<Mesh> HMesh;
	typedef ResourceHandle<GpuProgram> HGpuProgram;
	typedef ResourceHandle<Material> HMaterial;
	typedef ResourceHandle<SamplerState> HSamplerState;
	typedef ResourceHandle<RasterizerState> HRasterizerState;
	typedef ResourceHandle<DepthStencilState> HDepthStencilState;
	typedef ResourceHandle<BlendState> HBlendState;
	typedef ResourceHandle<GpuProgInclude> HGpuProgInclude;
	typedef ResourceHandle<Font> HFont;
}

namespace BansheeEngine
{
	/**
	 * @brief	Defers function execution until the next frame. If this function is called
	 * 			within another deferred call, then it will be executed the same frame,
	 * 			but only after all existing deferred calls are done.
	 * 			
	 * @note	This method can be used for breaking dependencies among other things. If a class
	 * 			A depends on class B having something done, but class B also depends in some way on class A,
	 * 			you can break up the initialization into two separate steps, queuing the second step
	 * 			using this method.
	 * 			
	 *			Similar situation can happen if you have multiple classes being initialized in an undefined order
	 *			but some of them depend on others. Using this method you can defer the dependent step until next frame,
	 *			which will ensure everything was initialized.
	 *
	 * @param	callback	The callback.
	 */
	void BS_CORE_EXPORT deferredCall(std::function<void()> callback);

	// Special types for use by profilers
	typedef std::basic_string<char, std::char_traits<char>, StdAlloc<char, ProfilerAlloc>> ProfilerString;

	template <typename T, typename A = StdAlloc<T, ProfilerAlloc>>
	using ProfilerVector = std::vector<T, A>;

	template <typename T, typename A = StdAlloc<T, ProfilerAlloc>>
	using ProfilerStack = std::stack<T, std::deque<T, A>>;

	/**
	* @brief	Banshee thread policy that performs special startup/shutdown on threads
	*			managed by thread pool.
	*/
	class BS_CORE_EXPORT ThreadBansheePolicy
	{
	public:
		static void onThreadStarted(const String& name)
		{
			MemStack::beginThread();
		}

		static void onThreadEnded(const String& name)
		{
			MemStack::endThread();
		}
	};
}

#include "BsCommonTypes.h"