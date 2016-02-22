//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisitesUtil.h"

/** @defgroup Core Core
 *	Second lowest layer that provides some very game-specific modules tied into a coherent whole, but it tries to be very 
 *  generic and offer something that every engine might need instead of focusing on very specialized techniques.
 *  @{
 */

/** @defgroup CoreThread Core thread
 *	Functionality for dealing with core objects and interaction with the core thread.
 */

/** @defgroup Importer Importer
 *	Functionality for dealing with import of resources into engine friendly format.
 */

/** @defgroup Input Input
 *	Functionality for dealing with input (mouse, keyboard, gamepad, etc.).
 */

/** @defgroup Localization Localization
 *	Functionality for dealing with GUI localization.
 */

/** @defgroup Material Material
 *	Functionality for dealing with materials, shaders, and in general how objects are rendered.
 */

/** @defgroup Platform Platform
 *	Functionality specific for some platform (e.g. Windows, Mac).
 */

 /** @defgroup Profiling Profiling
  *	Functionality for measuring CPU and GPU execution times and memory usage.
  */

/** @defgroup RenderAPI RenderAPI
  *	Functionality for interacting with underlying render API (e.g. DirectX, OpenGL).
  */

/** @defgroup Renderer Renderer
  *	Abstract interface and helper functionality for rendering scene objects.
  */

/** @defgroup Resources Resources
  *	Contains core resource types and resource management functionality (loading, saving, etc.).
  */

/** @defgroup RTTI-Impl-Core RTTI types
 *  Types containing RTTI for specific classes.
 */

/** @defgroup Scene Scene
 *  Functionality for managing scene objects and their hierarchy.
 */

/** @defgroup Text Text
 *  Functionality for rendering text.
 */

/** @defgroup Utility-Core Utility
 *  Various utility methods and types used by the core layer.
 */

/** @defgroup Application-Core Application
 *  Entry point into the application and other general application functionality.
 */

/** @defgroup Components-Core Components
  *	Built-in components (elements that may be attached to scene objects).
  */

/** @defgroup Physics Physics
 *	Functionality for dealing with physics: colliders, triggers, rigidbodies, joints, scene queries, etc.
 */

/** @} */

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

#endif

// Linux/Apple Settings
#if BS_PLATFORM == BS_PLATFORM_LINUX || BS_PLATFORM == BS_PLATFORM_OSX

// Enable GCC symbol visibility
#   if defined( BS_GCC_VISIBILITY )
#       define BS_CORE_EXPORT  __attribute__ ((visibility("default")))
#       define BS_HIDDEN __attribute__ ((visibility("hidden")))
#   else
#       define BS_CORE_EXPORT
#       define BS_HIDDEN
#   endif

#endif

#include "BsHString.h"

namespace BansheeEngine 
{
	static const StringID RenderAPIAny = "AnyRenderAPI";
	static const StringID RendererAny = "AnyRenderer";

    class Color;
    class GpuProgram;
    class GpuProgramManager;
    class IndexBuffer;
	class IndexBufferCore;
    class OcclusionQuery;
    class VertexBuffer;
	class VertexBufferCore;
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
    class RenderAPICore;
    class RenderAPICapabilities;
    class RenderTarget;
	class RenderTargetCore;
    class RenderTexture;
	class RenderTextureCore;
	class MultiRenderTexture;
	class MultiRenderTextureCore;
    class RenderWindow;
	class RenderWindowCore;
	class RenderTargetProperties;
    struct RenderOpMesh;
    class StringInterface;
    class SamplerState;
	class SamplerStateCore;
    class TextureManager;
    class Viewport;
    class VertexData;
    class VertexDeclaration;
	class Input;
	struct PointerEvent;
	class RawInputHandler;
	class CoreRenderer;
	class RendererFactory;
	class AsyncOp;
	class HardwareBufferManager;
	class FontManager;
	class DepthStencilState;
	class DepthStencilStateCore;
	class RenderStateManager;
	class RasterizerState;
	class RasterizerStateCore;
	class BlendState;
	class BlendStateCore;
	class GpuParamBlock;
	class GpuParamBlockBuffer;
	class GpuParams;
	struct GpuParamDesc;
	struct GpuParamDataDesc;
	struct GpuParamObjectDesc;
	struct GpuParamBlockDesc;
	class ShaderInclude;
	class TextureView;
	class CoreObject;
	class CoreObjectCore;
	class ImportOptions;
	class TextureImportOptions;
	class FontImportOptions;
	class GpuProgramImportOptions;
	class MeshImportOptions;
	struct FontBitmap;
	class GameObject;
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
	class CameraCore;
	class MeshCoreBase;
	class MeshCore;
	struct SubMesh;
	class TransientMeshCore;
	class TextureCore;
	class MeshHeapCore;
	class VertexDeclarationCore;
	class GpuBufferCore;
	class GpuParamBlockBufferCore;
	class GpuParamsCore;
	class ShaderCore;
	class ViewportCore;
	class PassCore;
	class PassParametersCore;
	class TechniqueCore;
	class MaterialCore;
	class GpuProgramCore;
	class IResourceListener;
	class TextureProperties;
	class IShaderIncludeHandler;
	class Prefab;
	class PrefabDiff;
	class RendererMeshData;
	class LightCore;
	class Light;
	class Win32Window;
	class RenderAPIFactory;
	class PhysicsManager;
	class Physics;
	class FCollider;
	class Collider;
	class Rigidbody;
	class PhysicsMaterial;
	class BoxCollider;
	class SphereCollider;
	class PlaneCollider;
	class CapsuleCollider;
	class MeshCollider;
	class CCollider;
	class CRigidbody;
	class CBoxCollider;
	class CSphereCollider;
	class CPlaneCollider;
	class CCapsuleCollider;
	class CMeshCollider;
	class Joint;
	class FixedJoint;
	class DistanceJoint;
	class HingeJoint;
	class SphericalJoint;
	class SliderJoint;
	class D6Joint;
	class CharacterController;
	class CJoint;
	class CHingeJoint;
	class CDistanceJoint;
	class CFixedJoint;
	class CSphericalJoint;
	class CSliderJoint;
	class CD6Joint;
	class CCharacterController;
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
	class StringTable;
	class PhysicsMaterial;
	class PhysicsMesh;
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
	struct CHAR_CONTROLLER_DESC;

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
	typedef std::shared_ptr<RenderAPICore> RenderAPIPtr;
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
	typedef std::shared_ptr<CoreRenderer> CoreRendererPtr;
	typedef std::shared_ptr<RendererFactory> RendererFactoryPtr;
	typedef std::shared_ptr<Component> ComponentPtr;
	typedef std::shared_ptr<GameObject> GameObjectPtr;
	typedef std::shared_ptr<SceneObject> SceneObjectPtr;
	typedef std::shared_ptr<SamplerState> SamplerStatePtr;
	typedef std::shared_ptr<DepthStencilState> DepthStencilStatePtr;
	typedef std::shared_ptr<RasterizerState> RasterizerStatePtr;
	typedef std::shared_ptr<BlendState> BlendStatePtr;
	typedef std::shared_ptr<RenderWindow> RenderWindowPtr;
	typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
	typedef std::shared_ptr<RenderTexture> RenderTexturePtr;
	typedef std::shared_ptr<MultiRenderTexture> MultiRenderTexturePtr;
	typedef std::shared_ptr<GpuParamBlockBuffer> GpuParamBlockBufferPtr;
	typedef std::shared_ptr<GpuParams> GpuParamsPtr;
	typedef std::shared_ptr<TextureView> TextureViewPtr;
	typedef std::shared_ptr<Viewport> ViewportPtr;
	typedef std::shared_ptr<ShaderInclude> ShaderIncludePtr;
	typedef std::shared_ptr<ImportOptions> ImportOptionsPtr;
	typedef std::shared_ptr<const ImportOptions> ConstImportOptionsPtr;
	typedef std::shared_ptr<Font> FontPtr;
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
	typedef std::shared_ptr<RenderQueue> RenderQueuePtr;
	typedef std::shared_ptr<GpuParamDesc> GpuParamDescPtr;
	typedef std::shared_ptr<ResourceMetaData> ResourceMetaDataPtr;
	typedef std::shared_ptr<IShaderIncludeHandler> ShaderIncludeHandlerPtr;
	typedef std::shared_ptr<Prefab> PrefabPtr;
	typedef std::shared_ptr<PrefabDiff> PrefabDiffPtr;
	typedef std::shared_ptr<RendererMeshData> RendererMeshDataPtr;
	typedef std::shared_ptr<RenderAPIFactory> RenderAPIFactoryPtr;
	typedef std::shared_ptr<PhysicsMaterial> PhysicsMaterialPtr;
	typedef std::shared_ptr<PhysicsMesh> PhysicsMeshPtr;
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
		TID_SamplerState = 1021,
		TID_BlendState = 1023,
		TID_RasterizerState = 1024,
		TID_DepthStencilState = 1025,
		TID_BLEND_STATE_DESC = 1034,
		TID_SHADER_DATA_PARAM_DESC = 1035,
		TID_SHADER_OBJECT_PARAM_DESC = 1036,
		TID_SHADER_PARAM_BLOCK_DESC = 1047,
		TID_ImportOptions = 1048,
		TID_Font = 1051,
		TID_FONT_DESC = 1052,
		TID_CHAR_DESC = 1053,
		TID_FontImportOptions = 1056,
		TID_FontBitmap = 1057,
		TID_SceneObject = 1059,
		TID_GameObject = 1060,
		TID_PixelData = 1062,
		TID_GpuResourceData = 1063,
		TID_VertexDataDesc = 1064,
		TID_MeshBase = 1065,
		TID_GameObjectHandleBase = 1066,
		TID_ResourceManifest = 1067,
		TID_ResourceManifestEntry = 1068,
		TID_EmulatedParamBlock = 1069,
		TID_TextureImportOptions = 1070,
		TID_ResourceMetaData = 1071,
		TID_ShaderInclude = 1072,
		TID_Viewport = 1073,
		TID_ResourceDependencies = 1074,
		TID_ShaderMetaData = 1075,
		TID_MeshImportOptions = 1076,
		TID_Prefab = 1077,
		TID_PrefabDiff = 1078,
		TID_PrefabObjectDiff = 1079,
		TID_PrefabComponentDiff = 1080,
		TID_CGUIWidget = 1081,
		TID_ProfilerOverlay = 1082,
		TID_StringTable = 1083,
		TID_LanguageData = 1084,
		TID_LocalizedStringData = 1085,
		TID_MaterialParamColor = 1086,
		TID_WeakResourceHandle = 1087,
		TID_TextureParamData = 1088,
		TID_StructParamData = 1089,
		TID_MaterialParams = 1090,
		TID_MaterialRTTIParam = 1091,
		TID_PhysicsMaterial = 1092,
		TID_CCollider = 1093,
		TID_CBoxCollider = 1094,
		TID_CSphereCollider = 1095,
		TID_CCapsuleCollider = 1096,
		TID_CPlaneCollider = 1097,
		TID_CRigidbody = 1098,
		TID_PhysicsMesh = 1099,
		TID_CMeshCollider = 1100,
		TID_CJoint = 1101,
		TID_CFixedJoint = 1102,
		TID_CDistanceJoint = 1103,
		TID_CHingeJoint = 1104,
		TID_CSphericalJoint = 1105,
		TID_CSliderJoint = 1106,
		TID_CD6Joint = 1107,
		TID_CCharacterController = 1108
	};
}

/************************************************************************/
/* 							Resource references                   		*/
/************************************************************************/

#include "BsResourceHandle.h"

namespace BansheeEngine
{
	/** @addtogroup Resources
	 *  @{
	 */

	typedef ResourceHandle<Resource> HResource;
	typedef ResourceHandle<Texture> HTexture;
	typedef ResourceHandle<Mesh> HMesh;
	typedef ResourceHandle<Material> HMaterial;
	typedef ResourceHandle<ShaderInclude> HShaderInclude;
	typedef ResourceHandle<Font> HFont;
	typedef ResourceHandle<Shader> HShader;
	typedef ResourceHandle<Prefab> HPrefab;
	typedef ResourceHandle<StringTable> HStringTable;
	typedef ResourceHandle<PhysicsMaterial> HPhysicsMaterial;
	typedef ResourceHandle<PhysicsMesh> HPhysicsMesh;

	/** @} */
}


#include "BsGameObjectHandle.h"

namespace BansheeEngine
{
	/** @addtogroup Scene
	 *  @{
	 */

	// Game object handles
	typedef GameObjectHandle<GameObject> HGameObject;
	typedef GameObjectHandle<SceneObject> HSceneObject;
	typedef GameObjectHandle<Component> HComponent;
	typedef GameObjectHandle<CRigidbody> HRigidbody;
	typedef GameObjectHandle<CCollider> HCollider;
	typedef GameObjectHandle<CBoxCollider> HBoxCollider;
	typedef GameObjectHandle<CSphereCollider> HSphereCollider;
	typedef GameObjectHandle<CCapsuleCollider> HCapsuleCollider;
	typedef GameObjectHandle<CPlaneCollider> HPlaneCollider;
	typedef GameObjectHandle<CJoint> HJoint;
	typedef GameObjectHandle<CHingeJoint> HHingeJoint;
	typedef GameObjectHandle<CSliderJoint> HSliderJoint;
	typedef GameObjectHandle<CDistanceJoint> HDistanceJoint;
	typedef GameObjectHandle<CSphericalJoint> HSphericalJoint;
	typedef GameObjectHandle<CFixedJoint> HFixedJoint;
	typedef GameObjectHandle<CD6Joint> HD6Joint;
	typedef GameObjectHandle<CCharacterController> HCharacterController;

	/** @} */
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

	#define BS_ALL_LAYERS 0xFFFFFFFFFFFFFFFF
}

#include "BsCommonTypes.h"