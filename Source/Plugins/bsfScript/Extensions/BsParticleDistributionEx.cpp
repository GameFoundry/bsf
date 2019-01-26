//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsParticleDistributionEx.h"

namespace bs
{
	Color ColorDistributionEx::evaluate(const SPtr<ColorDistribution>& thisPtr, float t, float factor)
	{
		return Color::fromRGBA(thisPtr->evaluate(t, factor));
	}

	class Color ColorDistributionEx::evaluate(const SPtr<ColorDistribution>& thisPtr, float t, Random& factor)
	{
		return Color::fromRGBA(thisPtr->evaluate(t, factor));
	}
}
