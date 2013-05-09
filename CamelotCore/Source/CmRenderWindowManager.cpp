#include "CmRenderWindowManager.h"

namespace CamelotFramework
{
	RenderWindowPtr RenderWindowManager::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		RenderWindowPtr renderWindow = createImpl(desc, parentWindow);
		renderWindow->setThisPtr(renderWindow);
		renderWindow->initialize();

		mCreatedWindows.push_back(renderWindow.get());
		
		if(!onWindowCreated.empty())
			onWindowCreated(renderWindow.get());

		return renderWindow;
	}

	void RenderWindowManager::windowDestroyed(RenderWindow* window)
	{
		auto iterFind = std::find(begin(mCreatedWindows), end(mCreatedWindows), window);

		if(iterFind == mCreatedWindows.end())
			CM_EXCEPT(InternalErrorException, "Trying to destroy a window that is not in the created windows list.");

		if(!onWindowDestroyed.empty())
			onWindowDestroyed(window);

		mCreatedWindows.erase(iterFind);
	}
}