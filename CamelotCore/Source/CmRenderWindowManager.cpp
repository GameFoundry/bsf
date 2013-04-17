#include "CmRenderWindowManager.h"

namespace CamelotFramework
{
	RenderWindowPtr RenderWindowManager::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		RenderWindowPtr renderWindow = createImpl(desc, parentWindow);
		renderWindow->setThisPtr(renderWindow);
		renderWindow->initialize();

		return renderWindow;
	}
}