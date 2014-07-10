//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLRenderSystemFactory.h"
#include "BsGLRenderSystem.h"

namespace BansheeEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp<GLRenderSystem>();
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}