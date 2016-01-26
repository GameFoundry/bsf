#pragma once

#include "BsPrerequisites.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	/**
	 * @brief	Holds data about a GUI event that happens when a virtual button is pressed.
	 */
	class BS_EXPORT GUIVirtualButtonEvent
	{
	public:
		GUIVirtualButtonEvent()
		{ }

		/**
		 * @brief	Returns the virtual button the event is referring to.
		 */
		const VirtualButton& getButton() const { return mButton; }
	private:
		friend class GUIManager;
		
		/**
		 * @brief	Initializes the data for the event.
		 */
		void setButton(const VirtualButton& button) { mButton = button; }

		VirtualButton mButton;
	};
}