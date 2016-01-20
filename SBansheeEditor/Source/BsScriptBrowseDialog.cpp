//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptBrowseDialog.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsPlatform.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	void ScriptBrowseDialog::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_OpenFile", &ScriptBrowseDialog::internal_OpenFile);
		metaData.scriptClass->addInternalCall("Internal_OpenFolder", &ScriptBrowseDialog::internal_OpenFolder);
		metaData.scriptClass->addInternalCall("Internal_SaveFile", &ScriptBrowseDialog::internal_SaveFile);
	}

	bool ScriptBrowseDialog::internal_OpenFile(MonoString* defaultFolder, MonoString* filterList, bool allowMultiselect, MonoArray** outPaths)
	{
		Path defaultFolderNative = MonoUtil::monoToWString(defaultFolder);
		WString filterListNative = MonoUtil::monoToWString(filterList);
		
		FileDialogType type = (FileDialogType)((UINT32)FileDialogType::OpenFile | (UINT32)FileDialogType::Multiselect);

		Vector<Path> paths;
		if (PlatformUtility::openBrowseDialog(type, defaultFolderNative, filterListNative, paths))
		{
			MonoArray* pathArray = mono_array_new(MonoManager::instance().getDomain(),
				mono_get_string_class(), (UINT32)paths.size());

			for (UINT32 i = 0; i < (UINT32)paths.size(); i++)
			{
				MonoString* monoString = MonoUtil::wstringToMono(paths[i].toWString());

				void* elemAddr = mono_array_addr_with_size(pathArray, sizeof(MonoString*), i);
				memcpy(elemAddr, &monoString, sizeof(MonoString*));
			}

			*outPaths = pathArray;
			return true;
		}
		else
		{
			*outPaths = nullptr;
			return false;
		}
	}

	bool ScriptBrowseDialog::internal_OpenFolder(MonoString* defaultFolder, MonoString** outPath)
	{
		Path defaultFolderNative = MonoUtil::monoToWString(defaultFolder);

		FileDialogType type = FileDialogType::OpenFolder;

		Vector<Path> paths;
		if (PlatformUtility::openBrowseDialog(type, defaultFolderNative, L"", paths))
		{
			if (paths.size() > 0)
				*outPath = MonoUtil::wstringToMono(paths[0].toWString());
			else
				*outPath = nullptr;

			return true;
		}
		else
		{
			*outPath = nullptr;
			return false;
		}
	}

	bool ScriptBrowseDialog::internal_SaveFile(MonoString* defaultFolder, MonoString* filterList, MonoString** outPath)
	{
		Path defaultFolderNative = MonoUtil::monoToWString(defaultFolder);
		WString filterListNative = MonoUtil::monoToWString(filterList);

		FileDialogType type = FileDialogType::Save;

		Vector<Path> paths;
		if (PlatformUtility::openBrowseDialog(type, defaultFolderNative, filterListNative, paths))
		{
			if (paths.size() > 0)
				*outPath = MonoUtil::wstringToMono(paths[0].toWString());
			else
				*outPath = nullptr;

			return true;
		}
		else
		{
			*outPath = nullptr;
			return false;
		}
	}
}