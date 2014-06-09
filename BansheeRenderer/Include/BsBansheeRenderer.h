#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsBounds.h"
#include "BsCameraProxy.h"

namespace BansheeEngine
{
	/**
	 * @brief	Default renderer for Banshee. Performs frustum culling, sorting and renders
	 *			objects plainly according to their shaders with no fancy effects.
	 *
	 */
	// TODO UNDOCUMENTED
	class BS_BSRND_EXPORT BansheeRenderer : public Renderer
	{
		struct CameraData
		{
			CameraProxyPtr cameraProxy;
			RenderQueuePtr renderQueue;
		};

		struct RenderTargetData
		{
			RenderTargetPtr target;
			Vector<CameraData> cameras;
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

	private:
		void addRenderableProxy(RenderableProxyPtr proxy);
		void removeRenderableProxy(RenderableProxyPtr proxy);
		void updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld);

		void addCameraProxy(CameraProxyPtr proxy);
		void removeCameraProxy(CameraProxyPtr proxy);
		void updateCameraProxy(CameraProxyPtr proxy, Matrix4 viewMatrix);

		void renderAllCore();

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 */
		virtual void render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue);

		void setPass(const MaterialProxyPass& pass);
		void draw(const MeshProxy& mesh);

		void renderableRemoved(const HRenderable& renderable);
		void cameraRemoved(const HCamera& camera);

		Vector<RenderableProxyPtr> mDeletedRenderableProxies;
		Vector<CameraProxyPtr> mDeletedCameraProxies;

		UnorderedMap<UINT64, RenderableProxyPtr> mRenderableProxies;
		UnorderedMap<UINT64, CameraProxyPtr> mCameraProxies;

		Vector<RenderTargetData> mRenderTargets;

		Vector<RenderableElement*> mRenderableElements;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		HEvent mRenderableRemovedConn;
		HEvent mCameraRemovedConn;
	};
}