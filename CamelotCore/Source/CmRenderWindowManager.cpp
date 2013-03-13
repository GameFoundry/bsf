#include "CmRenderWindowManager.h"

namespace CamelotEngine
{
	RenderWindowPtr RenderWindowManager::create(const RENDER_WINDOW_DESC& desc)
	{
		RenderWindowPtr renderWindow(createImpl(desc), &CoreObject::_deleteDelayed);
		renderWindow->setThisPtr(renderWindow);
		renderWindow->initialize();

		return renderWindow;
	}
}