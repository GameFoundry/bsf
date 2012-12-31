#include "CmRenderWindowManager.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"

namespace CamelotEngine
{
	RenderWindowPtr RenderWindowManager::create(const RENDER_WINDOW_DESC& desc)
	{
		RenderSystem* renderSystem = RenderSystem::instancePtr();
		AsyncOp op = renderSystem->queueReturnCommand(boost::bind(&RenderWindowManager::createImpl, this, desc, _1), true);

		return op.getReturnValue<RenderWindowPtr>();
	}
}