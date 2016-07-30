//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAnimationUtility.h"

namespace BansheeEngine
{
	void AnimationUtility::wrapTime(float& time, float start, float end, bool loop)
	{
		float length = end - start;

		// Clamp to start or loop
		if (time < start)
		{
			if (loop)
				time = time - std::floor(time / length) * length;
			else // Clamping
				time = start;
		}

		// Clamp to end or loop
		if (time > end)
		{
			if (loop)
				time = time - std::floor(time / length) * length;
			else // Clamping
				time = end;
		}
	}
}