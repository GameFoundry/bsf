//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLRenderWindowManager.h"
#include "BsGLRenderSystem.h"
#include "BsGLSupport.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	GLRenderWindowManager::GLRenderWindowManager(GLRenderSystem* renderSystem)
		:mRenderSystem(renderSystem)
	{
		assert(mRenderSystem != nullptr);
	}

	RenderWindowPtr GLRenderWindowManager::createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		GLSupport* glSupport = mRenderSystem->getGLSupport();

		// Create the window
		return glSupport->newWindow(desc, parentWindow);
	}
}