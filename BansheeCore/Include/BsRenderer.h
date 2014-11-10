#pragma once

#include "BsCorePrerequisites.h"
#include "BsGameObject.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Available parameter block semantics that allow the renderer to identify
	 *			the use of a GPU program parameter block specified in a shader.
	 */
	enum RendererBlockSemantic
	{
		// 0 - Reserved
		RBS_Static = 1,
		RBS_PerCamera = 2,
		RBS_PerFrame = 3,
		RBS_PerObject = 4
	};

	/**
	 * @brief	Available parameter semantics that allow the renderer to identify
	 *			the use of a GPU parameter specified in a shader.
	 */
	enum RendererParamSemantic
	{
		// 0 - Reserved
		RPS_WorldViewProjTfrm = 1,
		RPS_ViewProjTfrm = 2,
		RPS_WorldTfrm = 3,
		RPS_MainTex = 4
	};

	/**
	 * @brief	Primarily rendering class that allows you to specify how to render objects that exist
	 *			in the scene graph. You need to provide your own implementation of your class.
	 *
	 * @note	Normally you would iterate over all cameras, find visible objects for each camera and render
	 *			those objects in some way.
	 */
	class BS_CORE_EXPORT Renderer
	{
	public:
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
		virtual const String& getName() const = 0;

		/**
		 * @brief	Called in order to render all currently active cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	Activates the specified pass on the pipeline.
		 *
		 * @param	material	Parent material of the pass.
		 * @param	passIdx		Index of the pass in the parent material.
		 *
		 * @note	Core thread only.
		 */
		static void setPass(const MaterialProxy& material, UINT32 passIdx);

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
		Event<void(const CameraProxy&)> onCorePreRenderViewport;

		/**
		 * @brief	Callback that gets triggered after main render queue items are rendered,
		 *			to the provided viewport, called from the core thread directly.
		 *
		 * @note	Core thread only.
		 */
		Event<void(const CameraProxy&)> onCorePostRenderViewport;
	};
}