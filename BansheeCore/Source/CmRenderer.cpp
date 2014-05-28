#include "CmRenderer.h"

namespace BansheeEngine
{
	void Renderer::addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, RenderQueue&)> callback)
	{
		mRenderCallbacks[viewport].push_back(callback);
	}
}