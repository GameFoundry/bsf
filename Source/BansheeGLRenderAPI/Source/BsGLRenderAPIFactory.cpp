//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGLRenderAPIFactory.h"
#include "BsGLRenderAPI.h"

namespace bs { namespace ct
{
	void GLRenderAPIFactory::create()
	{
		RenderAPI::startUp<GLRenderAPI>();
	}

	GLRenderAPIFactory::InitOnStart GLRenderAPIFactory::initOnStart;
}}