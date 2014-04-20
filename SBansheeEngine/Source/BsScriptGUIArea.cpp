#include "BsScriptGUIArea.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsScriptGUIPanel.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIArea::ScriptGUIArea(GUIArea* area, ScriptGUIPanel* parentGUI)
		:mGUIArea(area), mParentPanel(parentGUI), mIsDestroyed(false)
	{
		mParentPanel->registerArea(this);
	}

	void ScriptGUIArea::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIArea", &ScriptGUIArea::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIArea::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetArea", &ScriptGUIArea::internal_setArea);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIArea::internal_destroyInstance);
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

	void ScriptGUIArea::internal_createInstance(MonoObject* instance, MonoObject* panel, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth)
	{
		ScriptGUIPanel* scriptGUIPanel = ScriptGUIPanel::toNative(panel);
		GUIArea* nativeArea = GUIArea::create(scriptGUIPanel->getWidget(), x, y, width, height, depth);

		ScriptGUIArea* nativeInstance = new (cm_alloc<ScriptGUIArea>()) ScriptGUIArea(nativeArea, scriptGUIPanel);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIArea::internal_destroyInstance(ScriptGUIArea* thisPtr)
	{
		thisPtr->destroy();
		cm_delete(thisPtr);
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

	void ScriptGUIArea::internal_setArea(ScriptGUIArea* thisPtr, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth)
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
		myClippedArea.clip(parentArea);
		
		mGUIArea->setPosition(myClippedArea.x, myClippedArea.y);
		mGUIArea->setSize(myClippedArea.width, myClippedArea.height);
	}
}