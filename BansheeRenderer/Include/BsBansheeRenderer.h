#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderer.h"
#include "BsMaterial.h"
#include "BsGpuParamDesc.h"
#include "BsBounds.h"
#include "BsCameraProxy.h"

namespace BansheeEngine
{
	enum BansheeRendererParamSemantic
	{
		BRPS_Time = 1000,
		BRPS_LightDir = 1001
	};

	struct RenderableLitTexturedData
	{
		ShaderPtr defaultShader;

		GpuParamBlockDesc staticParamBlockDesc;
		GpuParamBlockDesc perFrameParamBlockDesc;
		GpuParamBlockDesc perObjectParamBlockDesc;

		GpuParamDataDesc timeParamDesc;
		GpuParamDataDesc lightDirParamDesc;
		GpuParamDataDesc wvpParamDesc;

		GpuParamBlockBufferPtr staticParamBuffer;
		GpuParamBlockBufferPtr perFrameParamBuffer;
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

	private:
		void addRenderableProxy(RenderableProxyPtr proxy);
		void removeRenderableProxy(RenderableProxyPtr proxy);
		void updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld);

		void addCameraProxy(CameraProxyPtr proxy);
		void removeCameraProxy(CameraProxyPtr proxy);
		void updateCameraProxy(CameraProxyPtr proxy, Matrix4 viewMatrix);

		void setDrawList(CameraProxyPtr proxy, DrawListPtr drawList);
		void updateMaterialProxy(MaterialProxyPtr proxy, Vector<PassParameters> dirtyPassParams);

		void renderAllCore();

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 */
		virtual void render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue);

		void setPass(const MaterialProxyPass& pass);
		void draw(const MeshProxy& mesh);

		void renderableRemoved(const HRenderable& renderable);
		void cameraRemoved(const HCamera& camera);

		ShaderPtr getDefaultShader(RenderableType type) const;

		Vector<RenderableProxyPtr> mDeletedRenderableProxies;
		Vector<CameraProxyPtr> mDeletedCameraProxies;

		UnorderedMap<UINT64, RenderableProxyPtr> mRenderableProxies;
		UnorderedMap<UINT64, CameraProxyPtr> mCameraProxies;

		Vector<RenderTargetData> mRenderTargets;

		Vector<RenderableElement*> mRenderableElements;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		RenderableLitTexturedData mLitTexturedData;

		HEvent mRenderableRemovedConn;
		HEvent mCameraRemovedConn;
	};
}