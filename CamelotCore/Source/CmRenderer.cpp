#include "CmRenderer.h"

namespace CamelotFramework
{
	void Renderer::addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, RenderQueue&)> callback)
	{
		mRenderCallbacks[viewport].push_back(callback);
	}
}