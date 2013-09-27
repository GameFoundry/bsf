#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownBox.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIDropDownBoxManager : public CM::Module<GUIDropDownBoxManager>
	{
	public:
		~GUIDropDownBoxManager();

		CM::GameObjectHandle<GUIDropDownBox> openDropDownBox(CM::Viewport* target, CM::RenderWindow* window, const GUIDropDownAreaPlacement& placement,
			const CM::Vector<GUIDropDownData>::type& elements, const GUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback);
		void closeDropDownBox();

	private:
		CM::HSceneObject mDropDownSO;
		CM::GameObjectHandle<GUIDropDownBox> mDropDownBox;
		std::function<void()> mOnClosedCallback;
	};
}