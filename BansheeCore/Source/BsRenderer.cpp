#include "BsRenderer.h"

namespace BansheeEngine
{
	void Renderer::addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, DrawList&)> callback)
	{
		mRenderCallbacks[viewport].push_back(callback);
	}
}