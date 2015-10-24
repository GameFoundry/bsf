#pragma once

#include "BsCorePrerequisites.h"
#include "BsGameObject.h"
#include "BsEvent.h"
#include "BsStringID.h"
#include "BsRendererMeshData.h"

namespace BansheeEngine
{
	/**
	 * Available parameter block semantics that allow the renderer to identify
	 * the use of a GPU program parameter block specified in a shader.
	 */
	static StringID RBS_Static = "Static";
	static StringID RBS_PerCamera = "PerCamera";
	static StringID RBS_PerFrame = "PerFrame";
	static StringID RBS_PerObject = "PerObject";

	/**
	 * Available parameter semantics that allow the renderer to identify
	 * the use of a GPU parameter specified in a shader.
	 */
	static StringID RPS_WorldViewProjTfrm = "WVP";
	static StringID RPS_ViewProjTfrm = "VP";
	static StringID RPS_ProjTfrm = "P";
	static StringID RPS_ViewTfrm = "V";
	static StringID RPS_WorldTfrm = "W";
	static StringID RPS_Diffuse = "Diffuse";
	static StringID RPS_ViewDir = "ViewDir";

	/**
	 * @brief	Set of options that can be used for controlling the renderer.
	 */	
	struct BS_CORE_EXPORT CoreRendererOptions
	{
		virtual ~CoreRendererOptions() { }
	};

	/**
	 * @brief	Primarily rendering class that allows you to specify how to render objects that exist
	 *			in the scene graph. You need to provide your own implementation of your class.
	 *
	 * @note	Normally you would iterate over all cameras, find visible objects for each camera and render
	 *			those objects in some way.
	 */
	class BS_CORE_EXPORT CoreRenderer
	{
	public:
		CoreRenderer();
		virtual ~CoreRenderer() { }

		/**
		 * @brief	Called after the renderer has been activated.
		 *
		 * @note	Internal method.
		 */
		virtual void _onActivated() { }

		/**
		 * @brief	Called just before the renderer is deactivated.
		 *
		 * @note	Internal method.
		 */
		virtual void _onDeactivated() { }

		/**
		 * @brief	Name of the renderer. Used by materials to find 
		 * 			an appropriate technique for this renderer.
		 */
		virtual const StringID& getName() const = 0;

		/**
		 * @brief	Called in order to render all currently active cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	Called whenever a new camera is created.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyCameraAdded(const CameraCore* camera) { }

		/**
		 * @brief	Called whenever a camera is destroyed.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyCameraRemoved(const CameraCore* camera) { }

		/**
		 * @brief	Creates a new empty renderer mesh data.
		 *
		 * @note	Sim thread.
		 *			Internal method.
		 */
		virtual RendererMeshDataPtr _createMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType = IT_32BIT);

		/**
		 * @brief	Creates a new renderer mesh data using an existing generic mesh data buffer.
		 *
		 * @note	Sim thread.
		 *			Internal method.
		 */
		virtual RendererMeshDataPtr _createMeshData(const MeshDataPtr& meshData);

		/**
		 * @brief	Registers a new callback that will be executed when the the specify camera is being rendered.
		 *
		 * @param	camera			Camera for which to trigger the callback.
		 * @param	index			Index that determines the order of rendering when there are multiple registered callbacks.
		 *							This must be unique. Lower indices get rendered sooner. Indices below 0 get rendered before the
		 *							main viewport elements, while indices equal or greater to zero, after.
		 * @param	callback		Callback to trigger when the specified camera is being rendered.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		void _registerRenderCallback(const CameraCore* camera, INT32 index, const std::function<void()>& callback);

		/**
		 * @brief	Removes a previously registered callback registered with "_registerRenderCallback".
		 */
		void _unregisterRenderCallback(const CameraCore* camera, INT32 index);

		/**
		 * @brief	Sets options used for controlling the rendering.
		 */
		virtual void setOptions(const SPtr<CoreRendererOptions>& options) { }

		/**
		 * @brief	Returns current set of options used for controlling the rendering.
		 */
		virtual SPtr<CoreRendererOptions> getOptions() const { return SPtr<CoreRendererOptions>(); }

		/**
		 * @brief	Activates the specified pass on the pipeline.
		 *
		 * @param	material	Parent material of the pass.
		 * @param	passIdx		Index of the pass in the parent material.
		 *
		 * @note	Core thread.
		 */
		static void setPass(const SPtr<MaterialCore>& material, UINT32 passIdx);

		/**
		 * @brief	Draws the specified mesh proxy with last set pass.
		 *
		 * @note	Core thread.
		 */
		static void draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh);

	protected:
		UnorderedMap<const CameraCore*, Map<UINT32, std::function<void()>>> mRenderCallbacks;
	};
}