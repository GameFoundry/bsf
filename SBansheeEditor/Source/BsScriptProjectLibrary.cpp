#include "BsScriptProjectLibrary.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsProjectLibrary.h"
#include "BsScriptResource.h"
#include "BsResources.h"
#include "BsResource.h"
#include "BsProjectResourceMeta.h"
#include "BsScriptResourceManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptFont.h"

namespace BansheeEngine
{
	ScriptProjectLibrary::ScriptProjectLibrary(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptProjectLibrary::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptProjectLibrary::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Load", &ScriptProjectLibrary::internal_Load);
		metaData.scriptClass->addInternalCall("Internal_Save", &ScriptProjectLibrary::internal_Save);
		metaData.scriptClass->addInternalCall("Internal_GetRoot", &ScriptProjectLibrary::internal_GetRoot);
		metaData.scriptClass->addInternalCall("Internal_Reimport", &ScriptProjectLibrary::internal_Reimport);
		metaData.scriptClass->addInternalCall("Internal_GetEntry", &ScriptProjectLibrary::internal_GetEntry);
		metaData.scriptClass->addInternalCall("Internal_GetPath", &ScriptProjectLibrary::internal_GetPath);
		metaData.scriptClass->addInternalCall("Internal_Delete", &ScriptProjectLibrary::internal_Delete);
		metaData.scriptClass->addInternalCall("Internal_CreateFolder", &ScriptProjectLibrary::internal_CreateFolder);
		metaData.scriptClass->addInternalCall("Internal_Rename", &ScriptProjectLibrary::internal_Rename);
		metaData.scriptClass->addInternalCall("Internal_Move", &ScriptProjectLibrary::internal_Move);
		metaData.scriptClass->addInternalCall("Internal_Copy", &ScriptProjectLibrary::internal_Copy);
	}

	void ScriptProjectLibrary::internal_Create(MonoObject* resource, MonoString* path)
	{
		ScriptResource* scrResource = ScriptResource::toNative(resource);
		Path resourcePath = MonoUtil::monoToWString(path);

		ProjectLibrary::instance().createEntry(scrResource->getNativeHandle(), resourcePath);
	}

	MonoObject* ScriptProjectLibrary::internal_Load(MonoString* path)
	{
		Path resourcePath = MonoUtil::monoToWString(path);

		Path fullPath = ProjectLibrary::instance().getResourcesFolder();
		fullPath.append(resourcePath);

		ProjectLibrary::LibraryEntry* entry = ProjectLibrary::instance().findEntry(fullPath);

		if (entry == nullptr || entry->type == ProjectLibrary::LibraryEntryType::Directory)
			return nullptr;

		ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
		String resUUID = resEntry->meta->getUUID();

		HResource resource = Resources::instance().loadFromUUID(resUUID);
		if (!resource)
			return nullptr;

		ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(resUUID);
		if (scriptResource == nullptr)
			scriptResource = ScriptResourceManager::instance().createScriptResource(resource);

		return scriptResource->getManagedInstance();
	}

	void ScriptProjectLibrary::internal_Save(MonoObject* resource)
	{

	}

	MonoObject* ScriptProjectLibrary::internal_GetRoot()
	{
		// TODO
	}

	void ScriptProjectLibrary::internal_Reimport(MonoString* path, MonoObject* options, bool force)
	{
		// TODO
	}

	MonoObject* ScriptProjectLibrary::internal_GetEntry(MonoString* path)
	{
		// TODO
	}

	MonoString* ScriptProjectLibrary::internal_GetPath(MonoObject* resource)
	{

	}

	void ScriptProjectLibrary::internal_Delete(MonoString* path)
	{
		Path pathToDelete = MonoUtil::monoToWString(path);
		ProjectLibrary::instance().deleteEntry(pathToDelete);
	}

	void ScriptProjectLibrary::internal_CreateFolder(MonoString* path)
	{
		Path folderToCreate = MonoUtil::monoToWString(path);
		ProjectLibrary::instance().createFolderEntry(folderToCreate);
	}

	void ScriptProjectLibrary::internal_Rename(MonoString* path, MonoString* name)
	{
		Path oldPath = MonoUtil::monoToWString(path);
		Path newPath = oldPath.getParent().append(MonoUtil::monoToWString(name));

		ProjectLibrary::instance().moveEntry(oldPath, newPath, false);
	}

	void ScriptProjectLibrary::internal_Move(MonoString* oldPath, MonoString* newPath, bool overwrite)
	{
		Path oldPathNative = MonoUtil::monoToWString(oldPath);
		Path newPathNative = MonoUtil::monoToWString(newPath);

		ProjectLibrary::instance().moveEntry(oldPathNative, newPathNative, overwrite);
	}

	void ScriptProjectLibrary::internal_Copy(MonoString* source, MonoString* destination, bool overwrite)
	{
		Path oldPathNative = MonoUtil::monoToWString(source);
		Path newPathNative = MonoUtil::monoToWString(destination);

		ProjectLibrary::instance().copyEntry(oldPathNative, newPathNative, overwrite);
	}
}