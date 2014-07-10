//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsBansheeRendererFactory.h"
#include "BsRenderer.h"
#include "BsBansheeRenderer.h"

namespace BansheeEngine
{
	RendererPtr BansheeRendererFactory::create()
	{
		return bs_shared_ptr<BansheeRenderer>();
	}

	const String& BansheeRendererFactory::name() const
	{
		return SystemName;
	}
}