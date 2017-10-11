//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptEditorBuiltin.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "Wrappers/BsScriptGUIContentImages.h"
#include "BsScriptResourceManager.h"
#include "Wrappers/GUI/BsScriptGUISkin.h"

#include "BsScriptSpriteTexture.generated.h"

namespace bs
{
	ScriptEditorBuiltin::ScriptEditorBuiltin(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorBuiltin::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetLibraryItemIcon", (void*)&ScriptEditorBuiltin::internal_getLibraryItemIcon);
		metaData.scriptClass->addInternalCall("Internal_GetEmptyShaderCode", (void*)&ScriptEditorBuiltin::internal_GetEmptyShaderCode);
		metaData.scriptClass->addInternalCall("Internal_GetEmptyCSScriptCode", (void*)&ScriptEditorBuiltin::internal_GetEmptyCSScriptCode);
		metaData.scriptClass->addInternalCall("Internal_GetToolbarIcon", (void*)&ScriptEditorBuiltin::internal_GetToolbarIcon);
		metaData.scriptClass->addInternalCall("Internal_GetLibraryWindowIcon", (void*)&ScriptEditorBuiltin::internal_GetLibraryWindowIcon);
		metaData.scriptClass->addInternalCall("Internal_GetInspectorWindowIcon", (void*)&ScriptEditorBuiltin::internal_GetInspectorWindowIcon);
		metaData.scriptClass->addInternalCall("Internal_GetSceneWindowIcon", (void*)&ScriptEditorBuiltin::internal_GetSceneWindowIcon);
		metaData.scriptClass->addInternalCall("Internal_GetAnimationWindowIcon", (void*)&ScriptEditorBuiltin::internal_GetAnimationWindowIcon);
		metaData.scriptClass->addInternalCall("Internal_GetLogIcon", (void*)&ScriptEditorBuiltin::internal_GetLogIcon);
		metaData.scriptClass->addInternalCall("Internal_GetEditorIcon", (void*)&ScriptEditorBuiltin::internal_GetEditorIcon);
		metaData.scriptClass->addInternalCall("Internal_GetDefaultFont", (void*)&ScriptEditorBuiltin::internal_GetDefaultFont);
		metaData.scriptClass->addInternalCall("Internal_GetGUISkin", (void*)&ScriptEditorBuiltin::internal_GetGUISkin);
	}

	MonoObject* ScriptEditorBuiltin::internal_getLibraryItemIcon(ProjectIcon icon, int size)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(icon, size);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}

	MonoString* ScriptEditorBuiltin::internal_GetEmptyShaderCode()
	{
		WString code = BuiltinEditorResources::instance().getEmptyShaderCode();

		return MonoUtil::wstringToMono(code);
	}

	MonoString* ScriptEditorBuiltin::internal_GetEmptyCSScriptCode()
	{
		WString code = BuiltinEditorResources::instance().getEmptyCSScriptCode();

		return MonoUtil::wstringToMono(code);
	}

	MonoObject* ScriptEditorBuiltin::internal_GetToolbarIcon(ToolbarIcon icon)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getToolbarIcon(icon);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}

	MonoObject* ScriptEditorBuiltin::internal_GetLibraryWindowIcon(LibraryWindowIcon icon)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryWindowIcon(icon);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}

	MonoObject* ScriptEditorBuiltin::internal_GetInspectorWindowIcon(InspectorWindowIcon icon)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getInspectorWindowIcon(icon);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}

	MonoObject* ScriptEditorBuiltin::internal_GetSceneWindowIcon(SceneWindowIcon icon)
	{
		GUIContentImages images = BuiltinEditorResources::instance().getSceneWindowIcon(icon);

		return ScriptGUIContentImages::getManaged(images);
	}

	MonoObject* ScriptEditorBuiltin::internal_GetAnimationWindowIcon(AnimationWindowIcon icon)
	{
		GUIContentImages images = BuiltinEditorResources::instance().getAnimationWindowIcon(icon);

		return ScriptGUIContentImages::getManaged(images);
	}

	MonoObject* ScriptEditorBuiltin::internal_GetLogIcon(LogMessageIcon icon, int size, bool dark)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLogMessageIcon(icon, size, dark);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}

	MonoObject* ScriptEditorBuiltin::internal_GetEditorIcon(EditorIcon icon)
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getIcon(icon);
		return ScriptResourceManager::instance().getScriptResource(tex, true)->getManagedInstance();
	}
	
	MonoObject* ScriptEditorBuiltin::internal_GetDefaultFont()
	{
		HFont font = BuiltinEditorResources::instance().getDefaultFont();

		ScriptResourceBase* scriptFont = ScriptResourceManager::instance().getScriptResource(font, true);
		return scriptFont->getManagedInstance();
	}

	MonoObject* ScriptEditorBuiltin::internal_GetGUISkin()
	{
		HGUISkin guiSkin = BuiltinEditorResources::instance().getSkin();

		ScriptResourceBase* scriptGUISkin = ScriptResourceManager::instance().getScriptResource(guiSkin, true);
		return scriptGUISkin->getManagedInstance();
	}
}