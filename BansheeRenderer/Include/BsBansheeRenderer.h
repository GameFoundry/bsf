#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsBounds.h"
#include "BsRenderableElement.h"

namespace BansheeEngine
{
	/**
	 * Semantics that may be used for signaling the renderer
	 * for what is a certain shader parameter used for.
	 */

	static StringID RPS_Time = "Time";
	static StringID RPS_LightDir = "LightDir";

	/**
	 * @brief	Default renderer for Banshee. Performs frustum culling, sorting and 
	 *			renders objects in custom ways determine by renderable handlers.
	 *
	 * @note	Sim thread unless otherwise noted.
	 */
	class BS_BSRND_EXPORT BansheeRenderer : public Renderer
	{
		/**
		 * @brief	Render data for a single render target.
		 */
		struct RenderTargetData
		{
			SPtr<RenderTargetCore> target;
			Vector<const CameraHandlerCore*> cameras;
		};

		/**
		 * @brief	Data used by the renderer for a camera.
		 */
		struct CameraData
		{
			RenderQueuePtr renderQueue;
		};

		/**
		 * @brief	Data used by the renderer when rendering renderable handlers.
		 */
		struct RenderableData
		{
			RenderableHandlerCore* renderable;
			Vector<RenderableElement> elements;
			RenderableController* controller;
		};

	public:
		BansheeRenderer() { }
		~BansheeRenderer() { }

		/**
		 * @copydoc	Renderer::getName
		 */
		virtual const StringID& getName() const override;

		/**
		 * @copydoc	Renderer::renderAll
		 */
		virtual void renderAll() override;

		/**
		 * @copydoc	Renderer::_onActivated
		 */
		virtual void _onActivated() override;

		/**
		 * @copydoc	Renderer::_onDeactivated
		 */
		virtual void _onDeactivated() override;

	private:
		/**
		 * @copydoc	Renderer::_notifyCameraAdded
		 */
		void _notifyCameraAdded(const CameraHandlerCore* camera) override;

		/**
		 * @copydoc	Renderer::_notifyCameraRemoved
		 */
		void _notifyCameraRemoved(const CameraHandlerCore* camera) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableAdded
		 */
		void _notifyRenderableAdded(RenderableHandlerCore* renderable) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableUpdated
		 */
		void _notifyRenderableUpdated(RenderableHandlerCore* renderable) override;

		/**
		 * @copydoc	Renderer::_notifyRenderableRemoved
		 */
		void _notifyRenderableRemoved(RenderableHandlerCore* renderable) override;

		/**
		 * @brief	Adds a new set of objects to the cameras render queue.
		 *
		 * @param	proxy			Proxy of the render queues camera to add the objects to.
		 * @param	renderQueue		Objects to add to the cameras queue.
		 *
		 * @note	Core thread only.
		 */
		void addToRenderQueue(const SPtr<CameraHandlerCore>& proxy, RenderQueuePtr renderQueue);

		/**
		 * @brief	Performs rendering over all camera proxies.
		 *
		 * @param	time	Current frame time in milliseconds.
		 *
		 * @note	Core thread only.
		 */
		void renderAllCore(float time);

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 *
		 * @param	camera			Camera used for determining destination render target and visibility.
		 * @param	renderQueue		Optionally non-empty queue of manually added objects to render.
		 *
		 * @note	Core thread only.
		 */
		virtual void render(const CameraHandlerCore& camera, RenderQueuePtr& renderQueue);

		/**
		 * @brief	Creates data used by the renderer on the core thread.
		 */
		void initializeCore();

		/**
		 * @brief	Destroys data used by the renderer on the core thread.
		 */
		void destroyCore();

		/**
		 * @brief	Creates a dummy shader to be used when no other is available.
		 */
		SPtr<ShaderCore> createDefaultShader();

		Vector<RenderTargetData> mRenderTargets; // Core thread
		UnorderedMap<const CameraHandlerCore*, CameraData> mCameraData; // Core thread

		SPtr<MaterialCore> mDummyMaterial; // Core thread

		Vector<RenderableData> mRenderables; // Core thread
		Vector<Matrix4> mWorldTransforms; // Core thread
		Vector<Bounds> mWorldBounds; // Core thread

		LitTexRenderableController* mLitTexHandler;
	};
}