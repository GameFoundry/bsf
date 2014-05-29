#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderQueue.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Default implementation of a render queue. Just passes
	 * 			through elements unmodified as they were queued in.
	 */
	class BS_CORE_EXPORT DefaultRenderQueue : public RenderQueue
	{
	public:
		void sort();
	};
}