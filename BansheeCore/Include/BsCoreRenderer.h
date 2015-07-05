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
	static StringID RPS_WorldTfrm = "World";
	static StringID RPS_Diffuse = "Diffuse";

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
		virtual void _notifyCameraAdded(const CameraHandlerCore* camera) { }

		/**
		 * @brief	Called whenever a camera is destroyed.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyCameraRemoved(const CameraHandlerCore* camera) { }

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
		 * @brief	Activates the specified pass on the pipeline.
		 *
		 * @param	material	Parent material of the pass.
		 * @param	passIdx		Index of the pass in the parent material.
		 *
		 * @note	Core thread only.
		 */
		static void setPass(const SPtr<MaterialCore>& material, UINT32 passIdx);

		/**
		 * @brief	Draws the specified mesh proxy with last set pass.
		 *
		 * @note	Core thread only.
		 */
		static void draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh);

		/**
		 * @brief	Callback that gets triggered before a viewport gets rendered.
		 *
		 * @note	Sim thread only
		 */
		Event<void(const Viewport*, DrawList&)> onRenderViewport;

		/**
		 * @brief	Callback that gets triggered before main render queue items are rendered
		 *			to the provided viewport, called from the core thread directly.
		 *
		 * @note	Core thread only.
		 */
		Event<void(const CameraHandlerCore&)> onCorePreRenderViewport;

		/**
		 * @brief	Callback that gets triggered after main render queue items are rendered,
		 *			to the provided viewport, called from the core thread directly.
		 *
		 * @note	Core thread only.
		 */
		Event<void(const CameraHandlerCore&)> onCorePostRenderViewport;
	};
}