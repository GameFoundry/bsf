#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRenderableHandler.h"
#include "BsMaterialProxy.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_BSRND_EXPORT LitTexRenderableHandler : public RenderableHandler
	{
	public:
		struct PerObjectData
		{
			GpuParamBlockBufferPtr perObjectParamBuffer;

			bool hasWVPParam;
			GpuParamMat4 wvpParam;

			Vector<MaterialProxy::BufferBindInfo> perObjectBuffers;
		};

		LitTexRenderableHandler();

		void initializeProxy(const RenderableProxyPtr& proxy);

		void bindPerObjectBuffers(const RenderableProxyPtr& renderable, const RenderableElement* element);

		void updateGlobalBuffers(float time);
		void updatePerObjectBuffers(RenderableElement* element, const Matrix4& wvpMatrix);

	protected:
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
}