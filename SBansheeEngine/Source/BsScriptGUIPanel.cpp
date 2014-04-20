#include "BsScriptGUIPanel.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIPanel::ScriptGUIPanel()
	{

	}

	void ScriptGUIPanel::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "GUIPanel", &ScriptGUIPanel::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIPanel::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIPanel::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIPanel::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetArea", &ScriptGUIPanel::internal_setArea);
	}

	void ScriptGUIPanel::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIPanel* nativeInstance = new (cm_alloc<ScriptGUIPanel>()) ScriptGUIPanel();
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptGUIPanel::internal_destroyInstance(ScriptGUIPanel* thisPtr)
	{
		cm_delete(thisPtr);
	}

	void ScriptGUIPanel::internal_setArea(ScriptGUIPanel* thisPtr, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth)
	{
		thisPtr->mMyArea.x = x;
		thisPtr->mMyArea.y = y;
		thisPtr->mMyArea.width = width;
		thisPtr->mMyArea.height = height;

		thisPtr->updateArea();
	}

	void ScriptGUIPanel::setParentArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height)
	{
		mParentArea.x = x;
		mParentArea.y = y;
		mParentArea.width = width;
		mParentArea.height = height;

		updateArea();
	}

	void ScriptGUIPanel::setParentWidget(GUIWidget* widget) 
	{ 
		mParentWidget = widget; 

		for(auto& area : mAreas)
		{
			area->getInternalValue()->changeParentWidget(widget);
		}
	}

	void ScriptGUIPanel::registerArea(ScriptGUIArea* area)
	{
		mAreas.push_back(area);
	}

	void ScriptGUIPanel::unregisterArea(ScriptGUIArea* area)
	{
		auto iterFind = std::find(mAreas.begin(), mAreas.end(), area);

		if(iterFind != mAreas.end())
			mAreas.erase(iterFind);
	}

	void ScriptGUIPanel::updateArea()
	{
		mClippedArea = mMyArea;
		mClippedArea.x += mParentArea.x;
		mClippedArea.y += mParentArea.y;

		mClippedArea.clip(mParentArea);

		for(auto& area : mAreas)
		{
			area->updateArea();
		}
	}
}