//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Object that allows you to group multiple GUI toggle buttons.
	 *			Only one button among the grouped ones can be active.
	 */
	class BS_EXPORT GUIToggleGroup
	{
	public:
		~GUIToggleGroup();

		/**
		 * @brief	Registers a new toggle button with the group.
		 *
		 * @note	Internal method.
		 */
		void _add(GUIToggle* toggle);

		/**
		 * @brief	Unregisters a toggle button from the group.
		 *
		 * @note	Internal method.
		 */
		void _remove(GUIToggle* toggle);

	private:
		friend class GUIToggle;

		GUIToggleGroup(bool allowAllOff);

		/**
		 * @brief	Initializes the toggle group. To be called right after construction.
		 */
		void initialize(const std::shared_ptr<GUIToggleGroup>& sharedPtr);

		Vector<GUIToggle*> mButtons;
		bool mAllowAllOff;
		std::weak_ptr<GUIToggleGroup> mThis;
	};
}