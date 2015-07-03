#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	GUIDropDownBoxManager::~GUIDropDownBoxManager()
	{
		closeDropDownBox();
	}

	GameObjectHandle<GUIDropDownMenu> GUIDropDownBoxManager::openDropDownBox(Viewport* target, const DropDownAreaPlacement& placement,
		const GUIDropDownData& dropDownData, const HGUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback)
	{
		closeDropDownBox();

		mDropDownSO = SceneObject::create("DropDownBox", SOF_Internal | SOF_Persistent | SOF_DontSave);
		mDropDownBox = mDropDownSO->addComponent<GUIDropDownMenu>(target, placement, dropDownData, skin, type);
		mOnClosedCallback = onClosedCallback;

		return mDropDownBox;
	}

	void GUIDropDownBoxManager::closeDropDownBox()
	{
		if(mDropDownSO != nullptr)
		{
			mDropDownSO->destroy();
			mDropDownSO = nullptr;

			if(mOnClosedCallback != nullptr)
				mOnClosedCallback();

			mOnClosedCallback = nullptr;
		}
	}
}