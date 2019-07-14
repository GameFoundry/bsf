//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Wrappers/BsScriptContextMenu.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "GUI/BsGUIContextMenu.h"
#include "Wrappers/GUI/BsScriptGUILayout.h"
#include "GUI/BsGUILayout.h"

#include "Generated/BsScriptHString.generated.h"

using namespace std::placeholders;

namespace bs
{
	ScriptContextMenu::OnEntryTriggeredThunkDef ScriptContextMenu::onEntryTriggered;

	ScriptContextMenu::ScriptContextMenu(MonoObject* instance)
		: ScriptObject(instance)
	{
		mGCHandle = MonoUtil::newWeakGCHandle(instance);
		mContextMenu = bs_shared_ptr_new<GUIContextMenu>();
	}

	void ScriptContextMenu::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", (void*)&ScriptContextMenu::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_Open", (void*)&ScriptContextMenu::internal_Open);
		metaData.scriptClass->addInternalCall("Internal_AddItem", (void*)&ScriptContextMenu::internal_AddItem);
		metaData.scriptClass->addInternalCall("Internal_AddSeparator", (void*)&ScriptContextMenu::internal_AddSeparator);
		metaData.scriptClass->addInternalCall("Internal_SetLocalizedName", (void*)&ScriptContextMenu::internal_SetLocalizedName);

		onEntryTriggered = (OnEntryTriggeredThunkDef)metaData.scriptClass->getMethod("InternalDoOnEntryTriggered", 1)->getThunk();
	}

	void ScriptContextMenu::internal_CreateInstance(MonoObject* instance)
	{
		new (bs_alloc<ScriptContextMenu>()) ScriptContextMenu(instance);
	}

	void ScriptContextMenu::internal_Open(ScriptContextMenu* instance, Vector2I* position, ScriptGUILayout* layoutPtr)
	{
		GUIElementBase* layout = layoutPtr->getGUIElement();

		GUIWidget* widget = layout->_getParentWidget();
		if (widget == nullptr)
			return;

		Rect2I bounds = layout->getGlobalBounds();
		Vector2I windowPosition = *position + Vector2I(bounds.x, bounds.y);

		SPtr<GUIContextMenu> contextMenu = instance->getInternal();
		contextMenu->open(windowPosition, *widget);
	}

	void ScriptContextMenu::internal_AddItem(ScriptContextMenu* instance, MonoString* path, UINT32 callbackIdx,
		ShortcutKey* shortcut)
	{
		String nativePath = MonoUtil::monoToString(path);

		SPtr<GUIContextMenu> contextMenu = instance->getInternal();
		contextMenu->addMenuItem(nativePath, std::bind(&ScriptContextMenu::onContextMenuItemTriggered,
			instance, callbackIdx), 0, *shortcut);
	}

	void ScriptContextMenu::internal_AddSeparator(ScriptContextMenu* instance, MonoString* path)
	{
		String nativePath = MonoUtil::monoToString(path);

		SPtr<GUIContextMenu> contextMenu = instance->getInternal();
		contextMenu->addSeparator(nativePath, 0);
	}

	void ScriptContextMenu::internal_SetLocalizedName(ScriptContextMenu* instance, MonoString* label, ScriptHString* name)
	{
		if (label == nullptr || name == nullptr)
			return;

		String nativeLabel = MonoUtil::monoToString(label);
		SPtr<GUIContextMenu> contextMenu = instance->getInternal();
		contextMenu->setLocalizedName(nativeLabel, *name->getInternal());
	}

	void ScriptContextMenu::onContextMenuItemTriggered(UINT32 idx)
	{
		MonoObject* instance = MonoUtil::getObjectFromGCHandle(mGCHandle);
		MonoUtil::invokeThunk(onEntryTriggered, instance, idx);
	}
}
