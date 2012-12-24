#include "CmRenderWindowManager.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"

namespace CamelotEngine
{
	RenderWindowPtr RenderWindowManager::create(const String& name, UINT32 width, UINT32 height, 
		bool fullScreen, const map<String, String>::type* miscParams)
	{
		RenderSystem* renderSystem = RenderSystem::instancePtr();

		AsyncOp op;

		if(miscParams != nullptr)
			op = renderSystem->queueReturnCommand(boost::bind(&RenderWindowManager::createImpl, this, name, width, height, fullScreen, *miscParams, _1), true);
		else
			op = renderSystem->queueReturnCommand(boost::bind(&RenderWindowManager::createImpl, this, name, width, height, fullScreen, NameValuePairList(), _1), true);

		return op.getReturnValue<RenderWindowPtr>();
	}
}