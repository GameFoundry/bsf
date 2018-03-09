//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	struct GamepadInfo;

	/** Represents a single hardware gamepad. Used by the Input to report gamepad input events. */
	class BS_CORE_EXPORT Gamepad
	{
	public:
		struct Pimpl;

		Gamepad(const String& name, const GamepadInfo& gamepadInfo, Input* owner);
		~Gamepad();

		/** Returns the name of the device. */
		String getName() const { return mName; }

		/** Captures the input since the last call and triggers the events on the parent Input. */
		void capture();

		/** Minimum allowed value as reported by the axis movement events. */
		static constexpr int MIN_AXIS = -32768;

		/** Maximum allowed value as reported by the axis movement events. */
		static constexpr int MAX_AXIS = 32767;
	private:
		friend class Input;

		/** Changes the capture context. Should be called when focus is moved to a new window. */
		void changeCaptureContext(UINT64 windowHandle);

		String mName;
		Input* mOwner;

		Pimpl* m;
	};
}
