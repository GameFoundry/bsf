//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsColorGradientEx.h"

namespace bs
{
	Color ColorGradientEx::evaluate(const SPtr<ColorGradient>& thisPtr, float t)
	{
		return Color::fromRGBA(thisPtr->evaluate(t));
	}
}
