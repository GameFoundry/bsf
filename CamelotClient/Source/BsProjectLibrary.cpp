#include "BsProjectLibrary.h"
#include "BsEditorApplication.h"
#include "CmPath.h"
#include "CmFileSystem.h"
#include "CmException.h"
#include "CmResources.h"
#include "CmResourceManifest.h"
#include "CmImporter.h"
#include "BsResourceMeta.h"
#include "CmResources.h"
#include "CmImporter.h"
#include "CmImportOptions.h"
#include "CmFileSerializer.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const WString ProjectLibrary::INTERNAL_RESOURCES_DIR = L"Internal/Resources";

	enum class LibraryEntryType
	{
		File,
		Directory
	};

	struct ProjectLibrary::LibraryEntry
	{
		LibraryEntryType type;
		WPath path;

		DirectoryEntry* parent;
	};

	struct ProjectLibrary::ResourceEntry : public ProjectLibrary::LibraryEntry
	{
		ResourceMetaPtr meta;
		std::time_t lastUpdateTime;
	};

	struct ProjectLibrary::DirectoryEntry : public ProjectLibrary::LibraryEntry
	{
		Vector<LibraryEntry*>::type mChildren;
	};

	ProjectLibrary::ProjectLibrary()
		:mRootEntry(nullptr)
	{

	}

	ProjectLibrary::~ProjectLibrary()
	{
		if(mRootEntry != nullptr)
			deleteDirectory(mRootEntry);
	}

	void ProjectLibrary::checkForModifications(const CM::WString& folder)
	{
		if(!PathUtil::includes(folder, EditorApplication::instance().getResourcesFolderPath()))
			return; // Folder not part of our resources path, so no modifications

		if(mRootEntry == nullptr)
		{
			mRootEntry = cm_new<DirectoryEntry>();
			mRootEntry->path = toPath(EditorApplication::instance().getResourcesFolderPath());
		}

		DirectoryEntry* directoryEntry = findDirectoryEntry(folder);
		if(directoryEntry == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Attempting to check for modifications on a folder that is not yet part " \
				"of the project library. Maybe check for modifications on the parent folder first? Folder: \"" + toString(folder) + "\"");
		}

		Stack<DirectoryEntry*>::type todo;
		todo.push(directoryEntry);

		Vector<WPath>::type childFiles;
		Vector<WPath>::type childDirectories;
		Vector<bool>::type existingEntries;
		Vector<LibraryEntry*>::type toDelete;

		while(!todo.empty())
		{
			DirectoryEntry* currentDir = todo.top();
			todo.pop();

			existingEntries.clear();
			existingEntries.resize(currentDir->mChildren.size());
			for(UINT32 i = 0; i < (UINT32)currentDir->mChildren.size(); i++)
				existingEntries[i] = false;

			childFiles.clear();
			childDirectories.clear();

			FileSystem::getChildren(currentDir->path, childFiles, childDirectories);
			
			for(auto& filePath : childFiles)
			{
				ResourceEntry* existingEntry = nullptr;
				UINT32 idx = 0;
				for(auto& child : currentDir->mChildren)
				{
					if(child->type == LibraryEntryType::File && child->path == filePath)
					{
						existingEntries[idx] = true;
						existingEntry = static_cast<ResourceEntry*>(child);
						break;
					}

					idx++;
				}

				if(existingEntry != nullptr)
				{
					reimportResource(existingEntry);
				}
				else
				{
					addResource(currentDir, filePath);
				}
			}

			for(auto& dirPath : childDirectories)
			{
				DirectoryEntry* existingEntry = nullptr;
				UINT32 idx = 0;
				for(auto& child : currentDir->mChildren)
				{
					if(child->type == LibraryEntryType::Directory && child->path == dirPath)
					{
						existingEntries[idx] = true;
						existingEntry = static_cast<DirectoryEntry*>(child);
						break;
					}

					idx++;
				}

				if(existingEntry == nullptr)
				{
					DirectoryEntry* newEntry = cm_new<DirectoryEntry>();
					newEntry->path = dirPath;
					newEntry->type = LibraryEntryType::Directory;
					newEntry->parent = currentDir;

					currentDir->mChildren.push_back(newEntry);
				}
			}

			{
				UINT32 idx = 0;
				toDelete.clear();
				for(auto& child : currentDir->mChildren)
				{
					if(existingEntries[idx])
						continue;

					toDelete.push_back(child);

					idx++;
				}

				for(auto& child : toDelete)
				{
					if(child->type == LibraryEntryType::Directory)
						deleteDirectory(static_cast<DirectoryEntry*>(child));
					else if(child->type == LibraryEntryType::File)
						deleteResource(static_cast<ResourceEntry*>(child));
				}
			}

			for(auto& child : currentDir->mChildren)
			{
				if(child->type == LibraryEntryType::Directory)
					todo.push(static_cast<DirectoryEntry*>(child));
			}
		}
	}

	void ProjectLibrary::addResource(DirectoryEntry* parent, const CM::WPath& filePath)
	{
		ResourceEntry* newResource = cm_new<ResourceEntry>();
		newResource->type = LibraryEntryType::File;
		newResource->path = filePath;
		newResource->parent = parent;

		parent->mChildren.push_back(newResource);

		reimportResource(newResource);
	}

	void ProjectLibrary::reimportResource(ResourceEntry* resource)
	{
		WString ext = toWString(PathUtil::getExtension(resource->path));
		WPath metaPath = resource->path;
		PathUtil::replaceExtension(metaPath, ext + L".meta");

		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if(!Importer::instance().supportsFileType(ext))
			return;

		if(resource->meta == nullptr)
		{
			FileSerializer fs;
			if(FileSystem::isFile(metaPath))
			{
				std::shared_ptr<IReflectable> loadedMeta = fs.decode(toWString(metaPath));

				if(loadedMeta != nullptr && loadedMeta->isDerivedFrom(ResourceMeta::getRTTIStatic()))
				{
					ResourceMetaPtr resourceMeta = std::static_pointer_cast<ResourceMeta>(loadedMeta);
					resource->meta = resourceMeta;
				}
			}
		}

		if(!isUpToDate(resource))
		{
			ImportOptionsPtr importOptions = nullptr;
			
			if(resource->meta != nullptr)
				importOptions = resource->meta->getImportOptions();
			else
				importOptions = Importer::instance().createImportOptions(toWString(resource->path));

			HResource importedResource = Importer::instance().import(toWString(resource->path), importOptions);

			if(resource->meta == nullptr)
			{
				resource->meta = ResourceMeta::create(importedResource.getUUID(), importOptions);
				FileSerializer fs;
				fs.encode(resource->meta.get(), toWString(metaPath)); // TODO - Make it accept WPath
			}

			WPath internalResourcesPath = toPath(EditorApplication::instance().getActiveProjectPath()) / toPath(INTERNAL_RESOURCES_DIR);
			if(!FileSystem::isDirectory(internalResourcesPath))
				FileSystem::createDir(internalResourcesPath);

			internalResourcesPath /= toPath(toWString(importedResource.getUUID()) + L".asset");

			gResources().save(importedResource, toWString(internalResourcesPath), true);
			resource->lastUpdateTime = std::time(nullptr);
		}
	}

	void ProjectLibrary::deleteResource(ResourceEntry* resource)
	{
		ResourceManifestPtr manifest = gResources().getResourceManifest();

		if(resource->meta != nullptr)
		{
			if(manifest->uuidExists(resource->meta->getUUID()))
			{
				const WString& path = manifest->uuidToFilePath(resource->meta->getUUID());
				if(FileSystem::isFile(path))
					FileSystem::remove(path);
			}

			WString ext = toWString(PathUtil::getExtension(resource->path));
			WPath metaPath = resource->path;
			PathUtil::replaceExtension(metaPath, ext + L".meta");

			if(FileSystem::isFile(metaPath))
				FileSystem::remove(metaPath);
		}

		DirectoryEntry* parent = resource->parent;
		auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(), 
			[&] (const LibraryEntry* entry) { return entry == resource; });

		parent->mChildren.erase(findIter);

		cm_delete(resource);
	}

	void ProjectLibrary::deleteDirectory(DirectoryEntry* directory)
	{
		if(directory == mRootEntry)
			mRootEntry = nullptr;

		for(auto& child : directory->mChildren)
		{
			if(child->type == LibraryEntryType::Directory)
				deleteDirectory(static_cast<DirectoryEntry*>(child));
			else
				deleteResource(static_cast<ResourceEntry*>(child));
		}

		DirectoryEntry* parent = directory->parent;
		auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(), 
			[&] (const LibraryEntry* entry) { return entry == directory; });

		parent->mChildren.erase(findIter);

		cm_delete(directory);
	}

	bool ProjectLibrary::isUpToDate(ResourceEntry* resource) const
	{
		if(resource->meta == nullptr)
			return false;

		ResourceManifestPtr manifest = gResources().getResourceManifest();

		if(!manifest->uuidExists(resource->meta->getUUID()))
			return false;

		const WString& path = manifest->uuidToFilePath(resource->meta->getUUID());
		if(!FileSystem::isFile(path))
			return false;

		std::time_t lastModifiedTime = FileSystem::getLastModifiedTime(path);
		return lastModifiedTime <= resource->lastUpdateTime;
	}

	ProjectLibrary::DirectoryEntry* ProjectLibrary::findDirectoryEntry(const CM::WString& folder) const
	{
		WPath pathToSearch = toPath(folder);

		Stack<DirectoryEntry*>::type todo;
		todo.push(mRootEntry);

		while(!todo.empty())
		{
			DirectoryEntry* currentDir = todo.top();
			todo.pop();

			for(auto& child : currentDir->mChildren)
			{
				if(child->type == LibraryEntryType::Directory)
				{
					if(pathToSearch == child->path)
						return static_cast<DirectoryEntry*>(child);

					todo.push(static_cast<DirectoryEntry*>(child));
				}
			}
		}

		return nullptr;
	}
}