#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	enum DeferredGpuCommandType
	{
		Draw,
		BeginFrame,
		EndFrame,
		Present
	};

	class CM_EXPORT DeferredGpuCommand
	{
	public:
		DeferredGpuCommandType commandType;
	};
}