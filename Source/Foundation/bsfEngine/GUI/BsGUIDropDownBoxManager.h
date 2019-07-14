//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "GUI/BsGUIDropDownMenu.h"
#include "Utility/BsModule.h"

namespace bs
{
	/** @addtogroup GUI-Internal
	 *  @{
	 */

	/**	Manages opening and closing of a drop down box. */
	class BS_EXPORT GUIDropDownBoxManager : public Module<GUIDropDownBoxManager>
	{
	public:
		~GUIDropDownBoxManager();

		/**
		 * Opens a new drop down box at the specified location, look and elements. This will close any previously open drop
		 * down box.
		 *
		 * @param[in]	desc				Various parameters for initializing the drop down box.
		 * @param[in]	type				Specific type of drop down box to display.
		 * @param[in]	onClosedCallback	Callback triggered when drop down box is closed.
		 */
		GameObjectHandle<GUIDropDownMenu> openDropDownBox(const DROP_DOWN_BOX_DESC& desc,
			GUIDropDownType type, std::function<void()> onClosedCallback);

		/**	Closes the currently active drop down box (if any). */
		void closeDropDownBox();

	private:
		HSceneObject mDropDownSO;
		GameObjectHandle<GUIDropDownMenu> mDropDownBox;
		std::function<void()> mOnClosedCallback;
	};

	/** @} */
}
