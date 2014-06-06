#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsBounds.h"
#include "BsCameraProxy.h"
#include "BsMaterialProxy.h"

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
			CameraProxy cameraProxy;
			RenderQueuePtr renderQueue;
		};

		struct RenderTargetData
		{
			RenderTargetPtr target;
			Vector<CameraData> cameras;
		};

		struct FrameData
		{
			Vector<RenderTargetData> renderTargets;
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
		void addRenderableProxy(RenderableProxy* proxy);
		void removeRenderableProxy(RenderableProxy* proxy);
		void updateRenderableProxy(RenderableProxy* proxy, Matrix4 localToWorld);

		void renderAllCore(std::shared_ptr<FrameData> frameData);

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 */
		virtual void render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue);

		void setPass(const MaterialProxy::PassData& pass);
		void draw(const MeshRenderData& mesh);

		void renderableRemoved(const HRenderable& renderable);

		Vector<RenderableProxy*> mDeletedProxies;

		Vector<RenderableSubProxy*> mRenderableProxies;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		HEvent mRenderableRemovedConn;
	};
}