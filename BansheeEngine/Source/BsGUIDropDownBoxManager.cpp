#include "BsGUIDropDownBoxManager.h"
#include "CmSceneObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIDropDownBoxManager::~GUIDropDownBoxManager()
	{
		closeDropDownBox();
	}

	CM::GameObjectHandle<GUIDropDownBox> GUIDropDownBoxManager::openDropDownBox(CM::Viewport* target, CM::RenderWindow* window, const GUIDropDownAreaPlacement& placement,
		const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback)
	{
		closeDropDownBox();

		mDropDownSO = SceneObject::create("DropDownBox");
		mDropDownBox = mDropDownSO->addComponent<GUIDropDownBox>(target, window, placement, dropDownData, skin, type);
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