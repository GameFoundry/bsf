#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownMenu.h"
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
		 * @param	desc				Various parameters for initializing the drop down box.
		 * @param	type				Specific type of drop down box to display.
		 * @param	onClosedCallback	Callback triggered when drop down box is closed.
		 */
		GameObjectHandle<GUIDropDownMenu> openDropDownBox(const DROP_DOWN_BOX_DESC& desc, 
			GUIDropDownType type, std::function<void()> onClosedCallback);

		/**
		 * @brief	Closes the currently active drop down box (if any).
		 */
		void closeDropDownBox();

	private:
		HSceneObject mDropDownSO;
		GameObjectHandle<GUIDropDownMenu> mDropDownBox;
		std::function<void()> mOnClosedCallback;
	};
}