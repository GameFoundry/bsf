//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	/** Represents a single hardware mouse. Used by the Input to report raw mouse input events. */
	class BS_CORE_EXPORT Mouse
	{
	public:
		struct Pimpl;

		Mouse(const String& name, Input* owner);
		~Mouse();

		/** Returns the name of the device. */
		String getName() const { return mName; }

		/** Captures the input since the last call and triggers the events on the parent Input. */
		void capture();

	private:
		friend class Input;

		/** Changes the capture context. Should be called when focus is moved to a new window. */
		void changeCaptureContext(UINT64 windowHandle);

		String mName;
		Input* mOwner;

		Pimpl* m;
	};
}
