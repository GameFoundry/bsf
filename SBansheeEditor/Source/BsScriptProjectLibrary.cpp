#include "BsScriptProjectLibrary.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptResource.h"
#include "BsResources.h"
#include "BsResource.h"
#include "BsProjectResourceMeta.h"
#include "BsScriptResourceManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptFont.h"
#include "BsScriptImportOptions.h"
#include "BsEditorApplication.h"
#include "BsPath.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptProjectLibrary::OnEntryChangedThunkDef ScriptProjectLibrary::OnEntryAddedThunk;
	ScriptProjectLibrary::OnEntryChangedThunkDef ScriptProjectLibrary::OnEntryRemovedThunk;

	HEvent ScriptProjectLibrary::mOnEntryAddedConn;
	HEvent ScriptProjectLibrary::mOnEntryRemovedConn;

	ScriptProjectLibrary::ScriptProjectLibrary(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptProjectLibrary::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Refresh", &ScriptProjectLibrary::internal_Refresh);
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptProjectLibrary::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Load", &ScriptProjectLibrary::internal_Load);
		metaData.scriptClass->addInternalCall("Internal_Save", &ScriptProjectLibrary::internal_Save);
		metaData.scriptClass->addInternalCall("Internal_GetRoot", &ScriptProjectLibrary::internal_GetRoot);
		metaData.scriptClass->addInternalCall("Internal_Reimport", &ScriptProjectLibrary::internal_Reimport);
		metaData.scriptClass->addInternalCall("Internal_GetEntry", &ScriptProjectLibrary::internal_GetEntry);
		metaData.scriptClass->addInternalCall("Internal_GetPath", &ScriptProjectLibrary::internal_GetPath);
		metaData.scriptClass->addInternalCall("Internal_GetPathFromUUID", &ScriptProjectLibrary::internal_GetPathFromUUID);
		metaData.scriptClass->addInternalCall("Internal_Search", &ScriptProjectLibrary::internal_Search);
		metaData.scriptClass->addInternalCall("Internal_Delete", &ScriptProjectLibrary::internal_Delete);
		metaData.scriptClass->addInternalCall("Internal_CreateFolder", &ScriptProjectLibrary::internal_CreateFolder);
		metaData.scriptClass->addInternalCall("Internal_Rename", &ScriptProjectLibrary::internal_Rename);
		metaData.scriptClass->addInternalCall("Internal_Move", &ScriptProjectLibrary::internal_Move);
		metaData.scriptClass->addInternalCall("Internal_Copy", &ScriptProjectLibrary::internal_Copy);
		metaData.scriptClass->addInternalCall("Internal_GetResourceFolder", &ScriptProjectLibrary::internal_GetResourceFolder);
		metaData.scriptClass->addInternalCall("Internal_SetIncludeInBuild", &ScriptProjectLibrary::internal_SetIncludeInBuild);

		OnEntryAddedThunk = (OnEntryChangedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnEntryAdded", 1)->getThunk();
		OnEntryRemovedThunk = (OnEntryChangedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnEntryRemoved", 1)->getThunk();
	}

	MonoArray* ScriptProjectLibrary::internal_Refresh(MonoString* path, bool import)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		if (!nativePath.isAbsolute())
			nativePath.makeAbsolute(gProjectLibrary().getResourcesFolder());

		Vector<Path> dirtyResources;
		gProjectLibrary().checkForModifications(nativePath, import, dirtyResources);

		ScriptArray output = ScriptArray::create<WString>((UINT32)dirtyResources.size());
		for (UINT32 i = 0; i < (UINT32)dirtyResources.size(); i++)
		{
			output.set(i, dirtyResources[i].toWString());
		}

		return output.getInternal();
	}

	void ScriptProjectLibrary::internal_Create(MonoObject* resource, MonoString* path)
	{
		ScriptResource* scrResource = ScriptResource::toNative(resource);
		Path resourcePath = MonoUtil::monoToWString(path);

		gProjectLibrary().createEntry(scrResource->getGenericHandle(), resourcePath);
	}

	MonoObject* ScriptProjectLibrary::internal_Load(MonoString* path)
	{
		Path resourcePath = MonoUtil::monoToWString(path);

		HResource resource;
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(resourcePath);
		if (entry != nullptr && entry->type == ProjectLibrary::LibraryEntryType::File)
		{
			ProjectLibrary::ResourceEntry* resEntry = static_cast <ProjectLibrary::ResourceEntry*>(entry);

			if (resEntry->meta != nullptr)
				resource = gResources().loadFromUUID(resEntry->meta->getUUID());
		}
		
		if (!resource)
			return nullptr;

		ScriptResourceBase* scriptResource;
		ScriptResourceManager::instance().getScriptResource(resource, &scriptResource, true);

		return scriptResource->getManagedInstance();
	}

	void ScriptProjectLibrary::internal_Save(MonoObject* resource)
	{
		ScriptResource* srcResource = ScriptResource::toNative(resource);

		if (srcResource != nullptr)
			gProjectLibrary().saveEntry(srcResource->getGenericHandle());
	}

	MonoObject* ScriptProjectLibrary::internal_GetRoot()
	{
		return ScriptDirectoryEntry::create(static_cast<const ProjectLibrary::DirectoryEntry*>(gProjectLibrary().getRootEntry()));
	}

	void ScriptProjectLibrary::internal_Reimport(MonoString* path, MonoObject* options, bool force)
	{
		Path assetPath = MonoUtil::monoToWString(path);

		SPtr<ImportOptions> nativeOptions;
		if (options != nullptr)
		{
			ScriptImportOptions* scriptOptions = ScriptImportOptions::toNative(options);
			nativeOptions = scriptOptions->getImportOptions();
		}

		gProjectLibrary().reimport(assetPath, nativeOptions, force);
	}

	MonoObject* ScriptProjectLibrary::internal_GetEntry(MonoString* path)
	{
		Path assetPath = MonoUtil::monoToWString(path);

		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(assetPath);
		if (entry == nullptr)
			return nullptr;

		if (entry->type == ProjectLibrary::LibraryEntryType::File)
			return ScriptFileEntry::create(static_cast<ProjectLibrary::ResourceEntry*>(entry));
		else
			return ScriptDirectoryEntry::create(static_cast<ProjectLibrary::DirectoryEntry*>(entry));
	}

	MonoString* ScriptProjectLibrary::internal_GetPathFromUUID(MonoString* uuid)
	{
		String nativeUUID = MonoUtil::monoToString(uuid);
		Path nativePath = gProjectLibrary().uuidToPath(nativeUUID);

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativePath.toWString());
	}

	MonoString* ScriptProjectLibrary::internal_GetPath(MonoObject* resource)
	{
		ScriptResource* srcResource = ScriptResource::toNative(resource);

		if (srcResource != nullptr)
		{
			Path nativePath = gProjectLibrary().uuidToPath(srcResource->getGenericHandle().getUUID());
			nativePath.getRelative(gProjectLibrary().getResourcesFolder());

			return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativePath.toWString());
		}

		return nullptr;
	}

	MonoArray* ScriptProjectLibrary::internal_Search(MonoString* pattern, MonoArray* types)
	{
		WString strPattern = MonoUtil::monoToWString(pattern);

		Vector<UINT32> typeIds;
		if (types != nullptr)
		{
			ScriptArray typeArray(types);
			for (UINT32 i = 0; i < typeArray.size(); i++)
			{
				UINT32 typeId = ScriptResource::getTypeIdFromType((ScriptResourceType)typeArray.get<UINT32>(i));
				typeIds.push_back(typeId);
			}
		}

		Vector<ProjectLibrary::LibraryEntry*> foundEntries = gProjectLibrary().search(strPattern, typeIds);

		UINT32 idx = 0;
		ScriptArray outArray = ScriptArray::create<ScriptLibraryEntry>((UINT32)foundEntries.size());
		for (auto& entry : foundEntries)
		{
			MonoObject* managedEntry = nullptr;

			if (entry->type == ProjectLibrary::LibraryEntryType::File)
				managedEntry = ScriptFileEntry::create(static_cast<ProjectLibrary::ResourceEntry*>(entry));
			else
				managedEntry = ScriptDirectoryEntry::create(static_cast<ProjectLibrary::DirectoryEntry*>(entry));

			outArray.set(idx, managedEntry);
			idx++;
		}

		return outArray.getInternal();
	}

	void ScriptProjectLibrary::internal_Delete(MonoString* path)
	{
		Path pathToDelete = MonoUtil::monoToWString(path);
		gProjectLibrary().deleteEntry(pathToDelete);
	}

	void ScriptProjectLibrary::internal_CreateFolder(MonoString* path)
	{
		Path folderToCreate = MonoUtil::monoToWString(path);
		gProjectLibrary().createFolderEntry(folderToCreate);
	}

	void ScriptProjectLibrary::internal_Rename(MonoString* path, MonoString* name, bool overwrite)
	{
		Path oldPath = MonoUtil::monoToWString(path);
		Path newPath = oldPath.getParent().append(MonoUtil::monoToWString(name));

		gProjectLibrary().moveEntry(oldPath, newPath, overwrite);
	}

	void ScriptProjectLibrary::internal_Move(MonoString* oldPath, MonoString* newPath, bool overwrite)
	{
		Path oldPathNative = MonoUtil::monoToWString(oldPath);
		Path newPathNative = MonoUtil::monoToWString(newPath);

		gProjectLibrary().moveEntry(oldPathNative, newPathNative, overwrite);
	}

	void ScriptProjectLibrary::internal_Copy(MonoString* source, MonoString* destination, bool overwrite)
	{
		Path oldPathNative = MonoUtil::monoToWString(source);
		Path newPathNative = MonoUtil::monoToWString(destination);

		gProjectLibrary().copyEntry(oldPathNative, newPathNative, overwrite);
	}

	MonoString* ScriptProjectLibrary::internal_GetResourceFolder()
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), gProjectLibrary().getResourcesFolder().toWString());
	}

	void ScriptProjectLibrary::internal_SetIncludeInBuild(MonoString* path, bool include)
	{
		Path pathNative = MonoUtil::monoToWString(path);

		gProjectLibrary().setIncludeInBuild(pathNative, include);
	}

	void ScriptProjectLibrary::startUp()
	{
		mOnEntryAddedConn = gProjectLibrary().onEntryAdded.connect(std::bind(&ScriptProjectLibrary::onEntryAdded, _1));
		mOnEntryRemovedConn = gProjectLibrary().onEntryRemoved.connect(std::bind(&ScriptProjectLibrary::onEntryRemoved, _1));
	}

	void ScriptProjectLibrary::shutDown()
	{
		mOnEntryAddedConn.disconnect();
		mOnEntryRemovedConn.disconnect();
	}

	void ScriptProjectLibrary::onEntryAdded(const Path& path)
	{
		Path relativePath = path;
		if (relativePath.isAbsolute())
			relativePath.makeRelative(gProjectLibrary().getResourcesFolder());

		MonoString* pathStr = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), relativePath.toWString());
		MonoUtil::invokeThunk(OnEntryAddedThunk, pathStr);
	}

	void ScriptProjectLibrary::onEntryRemoved(const Path& path)
	{
		Path relativePath = path;
		if (relativePath.isAbsolute())
			relativePath.makeRelative(gProjectLibrary().getResourcesFolder());

		MonoString* pathStr = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), relativePath.toWString());
		MonoUtil::invokeThunk(OnEntryRemovedThunk, pathStr);
	}

	ScriptLibraryEntryBase::ScriptLibraryEntryBase(MonoObject* instance)
		:ScriptObjectBase(instance)
	{ }

	void ScriptLibraryEntry::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetPath", &ScriptLibraryEntry::internal_GetPath);
		metaData.scriptClass->addInternalCall("Internal_GetName", &ScriptLibraryEntry::internal_GetName);
		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptLibraryEntry::internal_GetType);
		metaData.scriptClass->addInternalCall("Internal_GetParent", &ScriptLibraryEntry::internal_GetParent);
	}

	MonoString* ScriptLibraryEntry::internal_GetPath(ScriptLibraryEntryBase* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr)
			return nullptr;

		Path relativePath = entry->path;
		relativePath.makeRelative(gProjectLibrary().getResourcesFolder());

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), relativePath.toWString());
	}

	MonoString* ScriptLibraryEntry::internal_GetName(ScriptLibraryEntryBase* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr)
			return nullptr;

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), entry->elementName);
	}

	ProjectLibrary::LibraryEntryType ScriptLibraryEntry::internal_GetType(ScriptLibraryEntryBase* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr)
			return ProjectLibrary::LibraryEntryType::File; // Note: We don't actually know what this entry is, because it doesn't exist anymore

		return entry->type;
	}

	MonoObject* ScriptLibraryEntry::internal_GetParent(ScriptLibraryEntryBase* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->parent == nullptr)
			return nullptr;

		return ScriptDirectoryEntry::create(entry->parent);
	}

	ScriptDirectoryEntry::ScriptDirectoryEntry(MonoObject* instance, const Path& assetPath)
		:ScriptObject(instance)
	{
		mAssetPath = assetPath;
	}

	MonoObject* ScriptDirectoryEntry::create(const ProjectLibrary::DirectoryEntry* entry)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		bs_new<ScriptDirectoryEntry>(managedInstance, entry->path);

		return managedInstance;
	}

	void ScriptDirectoryEntry::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetChildren", &ScriptDirectoryEntry::internal_GetChildren);
	}

	MonoArray* ScriptDirectoryEntry::internal_GetChildren(ScriptDirectoryEntry* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->type != ProjectLibrary::LibraryEntryType::Directory)
			return ScriptArray::create<ScriptLibraryEntry>(0).getInternal();

		ProjectLibrary::DirectoryEntry* dirEntry = static_cast<ProjectLibrary::DirectoryEntry*>(entry);
		ScriptArray outArray = ScriptArray::create<ScriptLibraryEntry>((UINT32)dirEntry->mChildren.size());
		for (UINT32 i = 0; i < (UINT32)dirEntry->mChildren.size(); i++)
		{
			ProjectLibrary::LibraryEntry* childEntry = dirEntry->mChildren[i];
			MonoObject* managedChildEntry = nullptr;

			if (childEntry->type == ProjectLibrary::LibraryEntryType::File)
				managedChildEntry = ScriptFileEntry::create(static_cast<ProjectLibrary::ResourceEntry*>(childEntry));
			else
				managedChildEntry = ScriptDirectoryEntry::create(static_cast<ProjectLibrary::DirectoryEntry*>(childEntry));

			outArray.set(i, managedChildEntry);
		}

		return outArray.getInternal();
	}

	ScriptFileEntry::ScriptFileEntry(MonoObject* instance, const Path& assetPath)
		:ScriptObject(instance)
	{
		mAssetPath = assetPath;
	}

	MonoObject* ScriptFileEntry::create(const ProjectLibrary::ResourceEntry* entry)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();
		bs_new<ScriptFileEntry>(managedInstance, entry->path);

		return managedInstance;
	}

	void ScriptFileEntry::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetImportOptions", &ScriptFileEntry::internal_GetImportOptions);
		metaData.scriptClass->addInternalCall("Internal_GetUUID", &ScriptFileEntry::internal_GetUUID);
		metaData.scriptClass->addInternalCall("Internal_GetIcon", &ScriptFileEntry::internal_GetIcon);
		metaData.scriptClass->addInternalCall("Internal_GetResourceType", &ScriptFileEntry::internal_GetResourceType);
		metaData.scriptClass->addInternalCall("Internal_GetIncludeInBuild", &ScriptFileEntry::internal_GetIncludeInBuild);
	}

	MonoObject* ScriptFileEntry::internal_GetImportOptions(ScriptFileEntry* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->type != ProjectLibrary::LibraryEntryType::File)
			return nullptr;

		ProjectLibrary::ResourceEntry* fileEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);

		if (fileEntry->meta != nullptr)
			return ScriptImportOptions::create(fileEntry->meta->getImportOptions());
		else
			return nullptr;
	}

	MonoString* ScriptFileEntry::internal_GetUUID(ScriptFileEntry* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->type != ProjectLibrary::LibraryEntryType::File)
			return nullptr;

		ProjectLibrary::ResourceEntry* fileEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);

		if (fileEntry->meta != nullptr)
			return MonoUtil::stringToMono(MonoManager::instance().getDomain(), fileEntry->meta->getUUID());
		else
			return nullptr;
	}

	MonoObject* ScriptFileEntry::internal_GetIcon(ScriptFileEntry* thisPtr)
	{
		// TODO - Icons not supported yet
		return nullptr;
	}

	ScriptResourceType ScriptFileEntry::internal_GetResourceType(ScriptFileEntry* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->type != ProjectLibrary::LibraryEntryType::File)
			return ScriptResourceType::Undefined;

		ProjectLibrary::ResourceEntry* fileEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);

		if (fileEntry->meta != nullptr)
			return ScriptResource::getTypeFromTypeId(fileEntry->meta->getTypeID());

		return ScriptResourceType::Undefined;
	}

	bool ScriptFileEntry::internal_GetIncludeInBuild(ScriptFileEntry* thisPtr)
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(thisPtr->getAssetPath());
		if (entry == nullptr || entry->type != ProjectLibrary::LibraryEntryType::File)
			return false;

		ProjectLibrary::ResourceEntry* fileEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);

		if (fileEntry->meta != nullptr)
			return fileEntry->meta->getIncludeInBuild();

		return false;
	}
}