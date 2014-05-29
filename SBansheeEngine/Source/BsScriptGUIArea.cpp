#include "BsScriptGUIArea.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsScriptGUIPanel.h"

namespace BansheeEngine
{
	ScriptGUIArea::ScriptGUIArea(MonoObject* instance, GUIArea* area, ScriptGUIPanel* parentGUI)
		:ScriptObject(instance), mGUIArea(area), mParentPanel(parentGUI), mIsDestroyed(false)
	{
		mParentPanel->registerArea(this);
	}

	void ScriptGUIArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIArea::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetArea", &ScriptGUIArea::internal_setArea);
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptGUIArea::internal_destroy);
		metaData.scriptClass->addInternalCall("Internal_SetVisible", &ScriptGUIArea::internal_setVisible);
	}

	void ScriptGUIArea::destroy()
	{
		if(!mIsDestroyed)
		{
			mParentPanel->unregisterArea(this);

			GUIArea::destroy(mGUIArea);
			mGUIArea = nullptr;

			mIsDestroyed = true;
		}
	}

	void ScriptGUIArea::internal_createInstance(MonoObject* instance, MonoObject* panel, INT32 x, INT32 y, UINT32 width, UINT32 height, UINT16 depth)
	{
		ScriptGUIPanel* scriptGUIPanel = ScriptGUIPanel::toNative(panel);
		GUIArea* nativeArea = GUIArea::create(scriptGUIPanel->getWidget(), x, y, width, height, depth);

		ScriptGUIArea* nativeInstance = new (bs_alloc<ScriptGUIArea>()) ScriptGUIArea(instance, nativeArea, scriptGUIPanel);
	}

	void ScriptGUIArea::internal_destroy(ScriptGUIArea* thisPtr)
	{
		thisPtr->destroy();
	}

	void ScriptGUIArea::internal_setVisible(ScriptGUIArea* thisPtr, bool visible)
	{
		if(visible)
			thisPtr->mGUIArea->enable();
		else
			thisPtr->mGUIArea->disable();
	}

	void ScriptGUIArea::internal_setArea(ScriptGUIArea* thisPtr, INT32 x, INT32 y, UINT32 width, UINT32 height, UINT16 depth)
	{
		thisPtr->mArea.x = x;
		thisPtr->mArea.y = y;
		thisPtr->mArea.width = width;
		thisPtr->mArea.height = height;

		thisPtr->updateArea();
	}

	void ScriptGUIArea::updateArea()
	{
		RectI parentArea = mParentPanel->getClippedArea();
		RectI myClippedArea = mArea;
		myClippedArea.x += parentArea.x;
		myClippedArea.y += parentArea.y;
		myClippedArea.clip(parentArea);
		
		mGUIArea->setPosition(myClippedArea.x, myClippedArea.y);
		mGUIArea->setSize(myClippedArea.width, myClippedArea.height);
	}
}