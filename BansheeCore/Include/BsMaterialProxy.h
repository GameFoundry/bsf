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

		GpuParamsPtr vertexProgParams;
		GpuParamsPtr fragmentProgParams;
		GpuParamsPtr geometryProgParams;
		GpuParamsPtr hullProgParams;
		GpuParamsPtr domainProgParams;
		GpuParamsPtr computeProgParams;

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
		Vector<MaterialProxyPass> passes;
		bool separablePasses;
		UINT32 queuePriority;
		QueueSortType queueSortType;
	};
}