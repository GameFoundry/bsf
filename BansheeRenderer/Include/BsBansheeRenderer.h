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
			RenderTargetPtr target;
			Vector<CameraProxyPtr> cameras;
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
		 * @brief	Adds a new camera proxy will be used for rendering renderable proxy objects.
		 *
		 * @note	Core thread only.
		 */
		void addCameraProxy(CameraProxyPtr proxy);

		/**
		 * @brief	Removes an existing camera proxy, meaning the camera will no longer be rendered from.
		 * 
		 * @note	Core thread only.
		 */
		void removeCameraProxy(CameraProxyPtr proxy);

		/**
		 * @brief	Updates an existing camera proxy with new data. This includes data that changes
		 *			often. For other data it is best to remove old proxy and add new one.
		 *
		 * @param	proxy			Proxy to update.
		 * @param	worldPosition	World position of the camera.
		 * @param	worldMatrix		World transform matrix of the camera.
		 * @param	viewMatrix		View transform matrix of the camera.
		 *
		 * @note	Core thread only.
		 */
		void updateCameraProxy(CameraProxyPtr proxy, Vector3 worldPosition, Matrix4 worldMatrix, Matrix4 viewMatrix);

		/**
		 * @brief	Adds a new set of objects to the cameras render queue.
		 *
		 * @param	proxy			Proxy of the render queues camera to add the objects to.
		 * @param	renderQueue		Objects to add to the cameras queue.
		 *
		 * @note	Core thread only.
		 */
		void addToRenderQueue(CameraProxyPtr proxy, RenderQueuePtr renderQueue);

		/**
		 * @brief	Updates a material proxy with new parameter data. Usually called when parameters are manually
		 *			updated from the sim thread.
		 *
		 * @param	proxy		Material proxy to update.
		 * @param	dirtyParams	A list of parameter buffers that need updating.
		 *
		 * @note	Core thread only.
		 */
		void updateMaterialProxy(MaterialProxyPtr proxy, Vector<MaterialProxy::ParamsBindInfo> dirtyParams);

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
		 * @param	cameraProxy		Camera used for determining destination render target and visibility.
		 * @param	renderQueue		Optionally non-empty queue of manually added objects to render.
		 *
		 * @note	Core thread only.
		 */
		virtual void render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue);

		/**
		 * @brief	Activates the specified pass on the pipeline.
		 *
		 * @param	material	Parent material of the pass.
		 * @param	passIdx		Index of the pass in the parent material.
		 *
		 * @note	Core thread only.
		 */
		void setPass(const MaterialProxyPtr& material, UINT32 passIdx);

		/**
		 * @brief	Draws the specified mesh proxy with last set pass.
		 *
		 * @note	Core thread only.
		 */
		void draw(const MeshProxy& mesh);

		/**
		 * @brief	Called by the scene manager whenever a Renderable component has been
		 *			removed from the scene.
		 */
		void renderableRemoved(const HRenderable& renderable);

		/**
		 * @brief	Called by the scene manager whenever a Camera component has been
		 *			removed from the scene.
		 */
		void cameraRemoved(const HCamera& camera);

		Vector<RenderableProxyPtr> mDeletedRenderableProxies;
		Vector<CameraProxyPtr> mDeletedCameraProxies;

		UnorderedMap<UINT64, CameraProxyPtr> mCameraProxies;
		Vector<RenderTargetData> mRenderTargets;

		Vector<RenderableElement*> mRenderableElements;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		LitTexRenderableHandler* mLitTexHandler;

		HEvent mRenderableRemovedConn;
		HEvent mCameraRemovedConn;
	};
}