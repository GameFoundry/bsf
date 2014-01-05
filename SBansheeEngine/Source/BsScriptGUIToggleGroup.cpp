#include "BsScriptGUIToggleGroup.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUIToggle.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIToggleGroup::ScriptGUIToggleGroup(const std::shared_ptr<GUIToggleGroup>& toggleGroup)
		:mToggleGroup(toggleGroup)
	{

	}

	void ScriptGUIToggleGroup::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIToggleGroup", &ScriptGUIToggleGroup::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIToggleGroup::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIToggleGroup::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptGUIToggleGroup::internal_destroyInstance);
	}

	void ScriptGUIToggleGroup::internal_createInstance(MonoObject* instance)
	{
		std::shared_ptr<GUIToggleGroup> toggleGroup = GUIToggle::createToggleGroup();

		ScriptGUIToggleGroup* nativeInstance = new (cm_alloc<ScriptGUIToggleGroup>()) ScriptGUIToggleGroup(toggleGroup);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptGUIToggleGroup::internal_destroyInstance(ScriptGUIToggleGroup* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}