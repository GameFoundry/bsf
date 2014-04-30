#pragma once

#include "CmPrerequisites.h"
#include "CmRenderQueue.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Default implementation of a render queue. Just passes
	 * 			through elements unmodified as they were queued in.
	 */
	class CM_EXPORT DefaultRenderQueue : public RenderQueue
	{
	public:
		void sort();
	};
}