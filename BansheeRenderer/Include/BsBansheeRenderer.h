#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsMaterialProxy.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	/**
	 * @brief	Semantics that may be used for signaling the renderer
	 *			for what is a certain shader parameter used for.
	 */
	enum BansheeRendererParamSemantic
	{
		RPS_Time = 1000,
		RPS_LightDir = 1001
	};

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
		 * @brief	Various data used by a single camera.
		 */
		struct CameraData
		{
			RenderQueuePtr renderQueue;
		};

	public:
		BansheeRenderer();
		~BansheeRenderer();

		/**
		 * @copydoc	Renderer::getName
		 */
		virtual const String& getName() const;

		/**
		 * @copydoc	Renderer::renderAll
		 */
		virtual void renderAll();

		/**
		 * @copydoc	Renderer::_onActivated
		 */
		virtual void _onActivated();

		/**
		 * @copydoc	Renderer::_onDeactivated
		 */
		virtual void _onDeactivated();

	private:
		/**
		 * @copydoc	Renderer::_notifyCameraAdded
		 */
		virtual void _notifyCameraAdded(const CameraHandlerCore* camera);

		/**
		 * @copydoc	Renderer::_notifyCameraRemoved
		 */
		virtual void _notifyCameraRemoved(const CameraHandlerCore* camera);

		/**
		 * @brief	Adds a new renderable proxy which will be considered for rendering next frame.
		 *
		 * @note	Core thread only.
		 */
		void addRenderableProxy(RenderableProxyPtr proxy);

		/**
		 * @brief	Removes a previously existing renderable proxy so it will no longer be considered
		 *			for rendering.
		 *
		 * @note	Core thread only.
		 */
		void removeRenderableProxy(RenderableProxyPtr proxy);

		/**
		 * @brief	Updates an existing renderable proxy with new data. This includes data that changes
		 *			often. For other data it is best to remove old proxy and add new one.
		 *
		 * @param	proxy			Proxy to update.
		 * @param	localToWorld	Local to world transform of the parent Renderable.
		 *
		 * @note	Core thread only.
		 */
		void updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld);

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
		virtual void render(const CameraHandlerCore& camera, const RenderQueuePtr& renderQueue);

		/**
		 * @brief	Called by the scene manager whenever a Renderable component has been
		 *			removed from the scene.
		 */
		void renderableRemoved(const RenderableHandlerPtr& renderable);

		/**
		 * @brief	Creates data used by the renderer on the core thread.
		 */
		void initializeCore();

		/**
		 * @brief	Destroys data used by the renderer on the core thread.
		 */
		void destroyCore();

		Vector<RenderableProxyPtr> mDeletedRenderableProxies;

		Vector<RenderTargetData> mRenderTargets; // Core thread
		UnorderedMap<const CameraHandlerCore*, CameraData> mCameraData; // Core thread

		Vector<RenderableElement*> mRenderableElements;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		LitTexRenderableController* mLitTexHandler;

		HEvent mRenderableRemovedConn;
	};
}