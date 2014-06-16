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
		RPS_Time = 1000,
		RPS_LightDir = 1001
	};

	struct RenderLitTexturedGlobalData
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

		GpuParamsPtr staticParams;
		GpuParamsPtr perFrameParams;

		GpuParamVec4 lightDirParam;
		GpuParamFloat timeParam;
	};

	struct RenderLitTexturedPerObjectData
	{
		GpuParamBlockBufferPtr perObjectParamBuffer;

		bool hasWVPParam;
		GpuParamMat4 wvpParam;

		Vector<MaterialProxy::BufferBindInfo> perObjectBuffers;
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
		void initRenderableLitTexturedData();

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

		void bindGlobalBuffers(const RenderableElement* element);
		void bindPerObjectBuffers(const RenderableProxyPtr& renderable, const RenderableElement* element);

		ShaderPtr getDefaultShader(RenderableType type) const;

		Vector<RenderableProxyPtr> mDeletedRenderableProxies;
		Vector<CameraProxyPtr> mDeletedCameraProxies;

		UnorderedMap<UINT64, RenderableProxyPtr> mRenderableProxies;
		UnorderedMap<UINT64, CameraProxyPtr> mCameraProxies;

		Vector<RenderTargetData> mRenderTargets;

		Vector<RenderableElement*> mRenderableElements;
		Vector<Matrix4> mWorldTransforms;
		Vector<Bounds> mWorldBounds;

		RenderLitTexturedGlobalData mLitTexturedData;

		HEvent mRenderableRemovedConn;
		HEvent mCameraRemovedConn;
	};
}