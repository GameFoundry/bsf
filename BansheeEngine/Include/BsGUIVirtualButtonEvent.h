#pragma once

#include "BsPrerequisites.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIVirtualButtonEvent
	{
	public:
		GUIVirtualButtonEvent()
		{ }

		const VirtualButton& getButton() const { return mButton; }
	private:
		friend class GUIManager;

		VirtualButton mButton;

		void setButton(const VirtualButton& button) { mButton = button; }
	};
}