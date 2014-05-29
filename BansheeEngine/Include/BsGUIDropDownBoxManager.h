#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownBox.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIDropDownBoxManager : public Module<GUIDropDownBoxManager>
	{
	public:
		~GUIDropDownBoxManager();

		GameObjectHandle<GUIDropDownBox> openDropDownBox(Viewport* target, const GUIDropDownAreaPlacement& placement,
			const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback);
		void closeDropDownBox();

	private:
		HSceneObject mDropDownSO;
		GameObjectHandle<GUIDropDownBox> mDropDownBox;
		std::function<void()> mOnClosedCallback;
	};
}