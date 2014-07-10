//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownBox.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Managed opening and closing of a drop down box.
	 */
	class BS_EXPORT GUIDropDownBoxManager : public Module<GUIDropDownBoxManager>
	{
	public:
		~GUIDropDownBoxManager();

		/**
		 * @brief	Opens a new drop down box at the specified location, look and elements. This will close
		 *			any previously open drop down box.
		 *
		 * @param	target				Viewport on which to open the drop down box.
		 * @param	placement			Determines how is the drop down box positioned in the visible area.
		 * @param	dropDownData		Data to use for initializing menu items of the drop down box.
		 * @param	skin				Skin to use for drop down box GUI elements.
		 * @param	type				Specific type of drop down box to display.
		 * @param	onClosedCallback	Callback triggered when drop down box is closed.
		 */
		GameObjectHandle<GUIDropDownBox> openDropDownBox(Viewport* target, const GUIDropDownAreaPlacement& placement,
			const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback);

		/**
		 * @brief	Closes the currently active drop down box (if any).
		 */
		void closeDropDownBox();

	private:
		HSceneObject mDropDownSO;
		GameObjectHandle<GUIDropDownBox> mDropDownBox;
		std::function<void()> mOnClosedCallback;
	};
}