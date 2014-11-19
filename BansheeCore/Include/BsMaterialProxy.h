#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains data about a single pass in a material used by the
	 *			material proxy.
	 */
	struct BS_CORE_EXPORT MaterialProxyPass
	{
		HGpuProgram vertexProg;
		HGpuProgram fragmentProg;
		HGpuProgram geometryProg;
		HGpuProgram hullProg;
		HGpuProgram domainProg;
		HGpuProgram computeProg;

		UINT32 vertexProgParamsIdx;
		UINT32 fragmentProgParamsIdx;
		UINT32 geometryProgParamsIdx;
		UINT32 hullProgParamsIdx;
		UINT32 domainProgParamsIdx;
		UINT32 computeProgParamsIdx;

		HBlendState blendState;
		HRasterizerState rasterizerState;
		HDepthStencilState depthStencilState;
		UINT32 stencilRefValue;
	};

	/**
	 * @brief	Contains material information as seen by the core thread.
	 *			(Used for rendering and such.)
	 */
	struct BS_CORE_EXPORT MaterialProxy
	{
		/**
		 * @brief	Contains a hardware GPU parameter buffer and index of the parameters and the slot
		 *			it binds to in a material proxy.
		 */
		struct BS_CORE_EXPORT BufferBindInfo
		{
			BufferBindInfo(UINT32 paramsIdx, UINT32 slotIdx, const SPtr<GpuParamBlockBufferCore>& buffer)
				:paramsIdx(paramsIdx), slotIdx(slotIdx), buffer(buffer)
			{ }

			UINT32 paramsIdx;
			UINT32 slotIdx;
			SPtr<GpuParamBlockBufferCore> buffer;
		};

		Vector<MaterialProxyPass> passes;
		Vector<SPtr<GpuParamsCore>> params;

		ShaderProxyPtr shader;

		Vector<BufferBindInfo> rendererBuffers;
	};
}