#include "BsScriptGUIArea.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsScriptGUIBase.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIArea::ScriptGUIArea(GUIArea* area, ScriptGUIBase* parentGUI)
		:mArea(area), mParentGUI(parentGUI)
	{

	}

	void ScriptGUIArea::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIBase", &ScriptGUIArea::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptGUIArea::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIArea::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceResizableX", &ScriptGUIArea::internal_createInstanceResizeableX);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceResizableY", &ScriptGUIArea::internal_createInstanceResizeableY);
		metaData.scriptClass->addInternalCall("Internal_CreateInstanceResizableXY", &ScriptGUIArea::internal_createInstanceResizeableXY);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIArea::internal_destroyInstance);
	}

	GUIWidget& ScriptGUIArea::getParentWidget() const 
	{ 
		return mParentGUI->getWidget(); 
	}

	void ScriptGUIArea::internal_createInstance(MonoObject* instance, MonoObject* parentGUI, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth)
	{
		ScriptGUIBase* scriptGUIBase = ScriptGUIBase::toNative(parentGUI);
		GUIArea* nativeArea = GUIArea::create(scriptGUIBase->getWidget(), x, y, width, height, depth);

		ScriptGUIArea* nativeInstance = new (cm_alloc<ScriptGUIArea>()) ScriptGUIArea(nativeArea, scriptGUIBase);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIArea::internal_createInstanceResizeableX(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetLeft, CM::UINT32 offsetRight, 
		CM::UINT32 offsetTop, CM::UINT32 height, CM::UINT16 depth)
	{
		ScriptGUIBase* scriptGUIBase = ScriptGUIBase::toNative(parentGUI);
		GUIArea* nativeArea = GUIArea::createStretchedX(scriptGUIBase->getWidget(), offsetLeft, offsetRight, offsetTop, height, depth);
		
		ScriptGUIArea* nativeInstance = new (cm_alloc<ScriptGUIArea>()) ScriptGUIArea(nativeArea, scriptGUIBase);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIArea::internal_createInstanceResizeableY(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetTop, 
		CM::UINT32 offsetBottom, CM::UINT32 offsetLeft, CM::UINT32 width, CM::UINT16 depth)
	{
		ScriptGUIBase* scriptGUIBase = ScriptGUIBase::toNative(parentGUI);
		GUIArea* nativeArea = GUIArea::createStretchedY(scriptGUIBase->getWidget(), offsetTop, offsetBottom, offsetLeft, width, depth);

		ScriptGUIArea* nativeInstance = new (cm_alloc<ScriptGUIArea>()) ScriptGUIArea(nativeArea, scriptGUIBase);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIArea::internal_createInstanceResizeableXY(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetLeft, 
		CM::UINT32 offsetRight, CM::UINT32 offsetTop, CM::UINT32 offsetBottom, CM::UINT16 depth)
	{
		ScriptGUIBase* scriptGUIBase = ScriptGUIBase::toNative(parentGUI);
		GUIArea* nativeArea = GUIArea::createStretchedXY(scriptGUIBase->getWidget(), offsetLeft, offsetRight, offsetTop, offsetBottom, depth);

		ScriptGUIArea* nativeInstance = new (cm_alloc<ScriptGUIArea>()) ScriptGUIArea(nativeArea, scriptGUIBase);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIArea::internal_destroyInstance(ScriptGUIArea* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}