#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsMaterialProxy.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	enum BansheeRendererParamSemantic
	{
		RPS_Time = 1000,
		RPS_LightDir = 1001
	};

	/**
	 * @brief	Default renderer for Banshee. Performs frustum culling, sorting and renders
	 *			objects plainly according to their shaders with no fancy effects.
	 *
	 */
	// TODO UNDOCUMENTED
	class BS_BSRND_EXPORT BansheeRenderer : public Renderer
	{
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
		void addRenderableProxy(RenderableProxyPtr proxy);
		void removeRenderableProxy(RenderableProxyPtr proxy);
		void updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld);

		void addCameraProxy(CameraProxyPtr proxy);
		void removeCameraProxy(CameraProxyPtr proxy);
		void updateCameraProxy(CameraProxyPtr proxy, Matrix4 viewMatrix);

		void addToRenderQueue(CameraProxyPtr proxy, RenderQueuePtr renderQueue);
		void updateMaterialProxy(MaterialProxyPtr proxy, Vector<MaterialProxy::ParamsBindInfo> dirtyParams);

		void renderAllCore(float time);

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 */
		virtual void render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue);

		void setPass(const MaterialProxyPtr& material, UINT32 passIdx);
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

		LitTexRenderableHandler* mLitTexHandler;

		HEvent mRenderableRemovedConn;
		HEvent mCameraRemovedConn;
	};
}