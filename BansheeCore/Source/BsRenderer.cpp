//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsRenderer.h"

namespace BansheeEngine
{
	void Renderer::addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, DrawList&)> callback)
	{
		mRenderCallbacks[viewport].push_back(callback);
	}
}