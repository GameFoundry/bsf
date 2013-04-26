#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	class CM_EXPORT IGpuBuffer
	{
	public:
		virtual void setData(GpuBufferDataPtr data) = 0;
		virtual GpuBufferDataPtr getData() const = 0;
	};
}