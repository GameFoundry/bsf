#pragma once

#include "CmPrerequisites.h"
#include "CmCommandQueue.h"

namespace CamelotFramework
{	// TODO - This is duplicated here and in DeferredRenderContext
	template<class T>
	class DeferredRenderContext;

	typedef DeferredRenderContext<CommandQueueNoSync> RenderContext;
	typedef DeferredRenderContext<CommandQueueSync> SyncedRenderContext;
	typedef std::shared_ptr<DeferredRenderContext<CommandQueueNoSync>> RenderContextPtr;
	typedef std::shared_ptr<DeferredRenderContext<CommandQueueSync>> SyncedRenderContextPtr;
}