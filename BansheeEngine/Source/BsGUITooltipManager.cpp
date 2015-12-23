#include "BsGUITooltipManager.h"
#include "BsSceneObject.h"
#include "BsGUITooltip.h"

namespace BansheeEngine
{
	GUITooltipManager::~GUITooltipManager()
	{
		hide();
	}

	void GUITooltipManager::show(const CGUIWidget& widget, const Vector2I& position, const WString& text)
	{
		hide();

		mTooltipSO = SceneObject::create("Tooltip", SOF_Internal | SOF_Persistent | SOF_DontSave);
		GameObjectHandle<GUITooltip> tooltip = mTooltipSO->addComponent<GUITooltip>(widget, position, text);
	}

	void GUITooltipManager::hide()
	{
		if (mTooltipSO != nullptr)
		{
			mTooltipSO->destroy();
			mTooltipSO = nullptr;
		}
	}
}