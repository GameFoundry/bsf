#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT MaterialProxy
	{
	public:
		struct PassData
		{
			HGpuProgram vertexProg;
			HGpuProgram fragmentProg;
			HGpuProgram geometryProg;
			HGpuProgram hullProg;
			HGpuProgram domainProg;
			HGpuProgram computeProg;

			BindableGpuParamsPtr vertexProgParams;
			BindableGpuParamsPtr fragmentProgParams;
			BindableGpuParamsPtr geometryProgParams;
			BindableGpuParamsPtr hullProgParams;
			BindableGpuParamsPtr domainProgParams;
			BindableGpuParamsPtr computeProgParams;

			HBlendState blendState;
			HRasterizerState rasterizerState;
			HDepthStencilState depthStencilState;
			UINT32 stencilRefValue;
		};

		Vector<PassData> passes;
		bool separablePasses;
		UINT32 queuePriority;
		QueueSortType queueSortType;
	};
}