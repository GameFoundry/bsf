//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11RenderSystemFactory.h"
#include "BsRenderSystem.h"

namespace BansheeEngine
{
	void D3D11RenderSystemFactory::create()
	{
		RenderSystem::startUp<D3D11RenderSystem>();
	}

	D3D11RenderSystemFactory::InitOnStart D3D11RenderSystemFactory::initOnStart;
}