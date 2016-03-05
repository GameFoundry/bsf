//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptEditorPrerequisites.h"
#include "BsEditorScriptManager.h"
#include "BsEditorScriptLibrary.h"
#include "BsScriptManager.h"

namespace BansheeEngine
{
	extern "C" BS_SCR_BED_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEditor";
		return pluginName;
	}

	extern "C" BS_SCR_BED_EXPORT void* loadPlugin()
	{
		SPtr<EditorScriptLibrary> library = bs_shared_ptr_new<EditorScriptLibrary>();
		ScriptManager::instance()._setScriptLibrary(library);

		return nullptr;
	}

	extern "C" BS_SCR_BED_EXPORT void updatePlugin()
	{
		EditorScriptManager::instance().update();
	}

	extern "C" BS_SCR_BED_EXPORT void quitRequested()
	{
		EditorScriptManager::instance().quitRequested();
	}
}