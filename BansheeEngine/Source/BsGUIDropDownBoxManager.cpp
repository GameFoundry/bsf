//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIDropDownBoxManager.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	GUIDropDownBoxManager::~GUIDropDownBoxManager()
	{
		closeDropDownBox();
	}

	GameObjectHandle<GUIDropDownBox> GUIDropDownBoxManager::openDropDownBox(Viewport* target, const GUIDropDownAreaPlacement& placement,
		const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type, std::function<void()> onClosedCallback)
	{
		closeDropDownBox();

		mDropDownSO = SceneObject::create("DropDownBox");
		mDropDownBox = mDropDownSO->addComponent<GUIDropDownBox>(target, placement, dropDownData, skin, type);
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