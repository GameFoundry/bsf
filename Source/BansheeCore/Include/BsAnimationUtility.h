//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/** @addtogroup Animation
	 *  @{
	 */

	/** Helper class for dealing with animations, animation clips and curves. */
	class BS_CORE_EXPORT AnimationUtility : public CoreObject
	{
	public:
		/**
		 * Wraps or clamps the provided time value between the provided range.
		 *
		 * @param[in,out]	time	Time value to wrap/clamp.
		 * @param[in]		start	Start of the range.
		 * @param[in]		end		End of the range.
		 * @param[in]		loop	If true the value will be wrapped, otherwise clamped to range.
		 */
		static void wrapTime(float& time, float start, float end, bool loop);
	};

	/** @} */
}