#include "BsScriptGUIToggleGroup.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUIToggle.h"

namespace BansheeEngine
{
	ScriptGUIToggleGroup::ScriptGUIToggleGroup(MonoObject* instance, const std::shared_ptr<GUIToggleGroup>& toggleGroup)
		:ScriptObject(instance), mToggleGroup(toggleGroup)
	{

	}

	void ScriptGUIToggleGroup::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIToggleGroup::internal_createInstance);
	}

	void ScriptGUIToggleGroup::internal_createInstance(MonoObject* instance)
	{
		std::shared_ptr<GUIToggleGroup> toggleGroup = GUIToggle::createToggleGroup();

		ScriptGUIToggleGroup* nativeInstance = new (cm_alloc<ScriptGUIToggleGroup>()) ScriptGUIToggleGroup(instance, toggleGroup);
	}
}