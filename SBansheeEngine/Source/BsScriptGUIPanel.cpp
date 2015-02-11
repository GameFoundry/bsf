#include "BsScriptGUIPanel.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsMonoMethod.h"

namespace BansheeEngine
{
	MonoMethod* ScriptGUIPanel::mDestroyMethod = nullptr;

	ScriptGUIPanel::ScriptGUIPanel(MonoObject* instance)
		:ScriptObject(instance)
	{

	}

	void ScriptGUIPanel::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIPanel::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_SetArea", &ScriptGUIPanel::internal_setArea);

		mDestroyMethod = metaData.scriptClass->getMethod("Destroy", 0);
	}

	void ScriptGUIPanel::internal_createInstance(MonoObject* instance)
	{
		ScriptGUIPanel* nativeInstance = new (bs_alloc<ScriptGUIPanel>()) ScriptGUIPanel(instance);
	}

	void ScriptGUIPanel::internal_setArea(ScriptGUIPanel* thisPtr, INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		thisPtr->mMyArea.x = x;
		thisPtr->mMyArea.y = y;
		thisPtr->mMyArea.width = width;
		thisPtr->mMyArea.height = height;

		thisPtr->updateArea();
	}

	void ScriptGUIPanel::setParentArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
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

	void ScriptGUIPanel::destroy()
	{
		assert(mDestroyMethod != nullptr);

		mDestroyMethod->invoke(mManagedInstance, nullptr);
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