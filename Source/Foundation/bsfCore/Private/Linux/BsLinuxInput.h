//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Input/BsInputFwd.h"

namespace bs
{
	/** Infomation about an analog axis that's part of a gamepad. */
	struct AxisInfo
	{
		INT32 axisIdx;
		INT32 min;
		INT32 max;
	};

	/** Information about a gamepad. */
	struct GamepadInfo
	{
		UINT32 id;
		UINT32 eventHandlerIdx;
		String name;

		UnorderedMap<INT32, ButtonCode> buttonMap;
		UnorderedMap<INT32, AxisInfo> axisMap;
	};

	/**
	 * Data specific to Linux implementation of the input system. Can be passed to platform specific implementations of
	 * the individual device types.
	 */
	struct InputPrivateData
	{
		Vector<GamepadInfo> gamepadInfos;
	};

	/** Data about relative pointer / scroll wheel movement. */
	struct LinuxMouseMotionEvent
	{
		double deltaX; /**< Relative pointer movement in X direction. */
		double deltaY; /**< Relative pointer movement in Y direction. */
		double deltaZ; /**< Relative vertical scroll amount. */
	};

	/** Data about a single button press or release. */
	struct LinuxButtonEvent
	{
		UINT64 timestamp;
		ButtonCode button;
		bool pressed;
	};

#define BUFFER_SIZE_GAMEPAD 64
}

