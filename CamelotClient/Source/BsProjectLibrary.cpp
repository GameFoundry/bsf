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
#include "CmFolderMonitor.h"
#include "CmDebug.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const WString ProjectLibrary::INTERNAL_RESOURCES_DIR = L"Internal\\Resources";

	ProjectLibrary::LibraryEntry::LibraryEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent, LibraryEntryType type)
		:path(path), parent(parent), type(type), elementName(name)
	{ }

	ProjectLibrary::ResourceEntry::ResourceEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent)
		:LibraryEntry(path, name, parent, LibraryEntryType::File), lastUpdateTime(0)
	{ }

	ProjectLibrary::DirectoryEntry::DirectoryEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent)
		:LibraryEntry(path, name, parent, LibraryEntryType::Directory)
	{ }

	ProjectLibrary::ProjectLibrary()
		:mRootEntry(nullptr)
	{
		mMonitor = cm_new<FolderMonitor>();

		FolderChange folderChanges = (FolderChange)((UINT32)FolderChange::FileName | (UINT32)FolderChange::DirName | 
				(UINT32)FolderChange::Creation | (UINT32)FolderChange::LastWrite);
		mMonitor->startMonitor(EditorApplication::instance().getResourcesFolderPath(), true, folderChanges);

		mMonitor->onAdded.connect(boost::bind(&ProjectLibrary::onMonitorFileModified, this, _1));
		mMonitor->onRemoved.connect(boost::bind(&ProjectLibrary::onMonitorFileModified, this, _1));
		mMonitor->onModified.connect(boost::bind(&ProjectLibrary::onMonitorFileModified, this, _1));

		// TODO - Load
		checkForModifications(EditorApplication::instance().getResourcesFolderPath());
	}

	ProjectLibrary::~ProjectLibrary()
	{
		// TODO - Save

		mMonitor->stopMonitorAll();
		cm_delete(mMonitor);

		if(mRootEntry != nullptr)
			deleteDirectoryInternal(mRootEntry);
	}

	void ProjectLibrary::update()
	{
		mMonitor->update();
	}

	void ProjectLibrary::checkForModifications(const CM::WString& fullPath)
	{
		if(!PathUtil::includes(fullPath, EditorApplication::instance().getResourcesFolderPath()))
			return; // Folder not part of our resources path, so no modifications

		if(mRootEntry == nullptr)
		{
			WPath resPath = toPath(EditorApplication::instance().getResourcesFolderPath());
			mRootEntry = cm_new<DirectoryEntry>(resPath, toWString(PathUtil::getFilename(resPath)), nullptr);
		}

		WPath pathToSearch = toPath(fullPath);
		LibraryEntry* entry = findEntry(pathToSearch);
		if(entry == nullptr) // File could be new, try to find parent directory entry
		{
			WPath parentDirPath = PathUtil::parentPath(pathToSearch);
			entry = findEntry(parentDirPath);

			// Cannot find parent directory. Create the needed hierarchy.
			DirectoryEntry* entryParent = nullptr;
			DirectoryEntry* newHierarchyParent = nullptr;
			if(entry == nullptr) 
				createInternalParentHierarchy(pathToSearch, &newHierarchyParent, &entryParent);
			else
				entryParent = static_cast<DirectoryEntry*>(entry);

			if(FileSystem::isFile(pathToSearch))
			{
				addResourceInternal(entryParent, pathToSearch);
			}
			else if(FileSystem::isDirectory(pathToSearch))
			{
				addDirectoryInternal(entryParent, pathToSearch);

				if(newHierarchyParent == nullptr)
					checkForModifications(toWString(pathToSearch));
			}

			if(newHierarchyParent != nullptr)
				checkForModifications(toWString(newHierarchyParent->path));
		}
		else if(entry->type == LibraryEntryType::File)
		{
			if(FileSystem::isFile(entry->path))
			{
				reimportResourceInternal(static_cast<ResourceEntry*>(entry));
			}
			else
			{
				deleteResourceInternal(static_cast<ResourceEntry*>(entry));
			}
		}
		else if(entry->type == LibraryEntryType::Directory) // Check folder and all subfolders for modifications
		{
			if(!FileSystem::isDirectory(entry->path))
			{
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(entry));
			}
			else
			{
				Stack<DirectoryEntry*>::type todo;
				todo.push(static_cast<DirectoryEntry*>(entry));

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
						if(isMeta(filePath))
						{
							WPath sourceFilePath = filePath;
							PathUtil::replaceExtension(sourceFilePath, L"");

							if(FileSystem::isFile(sourceFilePath))
							{
								LOGWRN("Found a .meta file without a corresponding resource. Deleting.");

								FileSystem::remove(filePath);
							}
						}
						else
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
								reimportResourceInternal(existingEntry);
							}
							else
							{
								addResourceInternal(currentDir, filePath);
							}
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
							addDirectoryInternal(currentDir, dirPath);
					}

					{
						for(UINT32 i = 0; i < (UINT32)existingEntries.size(); i++)
						{
							if(existingEntries[i])
								continue;

							toDelete.push_back(currentDir->mChildren[i]);
						}

						for(auto& child : toDelete)
						{
							if(child->type == LibraryEntryType::Directory)
								deleteDirectoryInternal(static_cast<DirectoryEntry*>(child));
							else if(child->type == LibraryEntryType::File)
								deleteResourceInternal(static_cast<ResourceEntry*>(child));
						}
					}

					for(auto& child : currentDir->mChildren)
					{
						if(child->type == LibraryEntryType::Directory)
							todo.push(static_cast<DirectoryEntry*>(child));
					}
				}
			}
		}
	}

	ProjectLibrary::ResourceEntry* ProjectLibrary::addResourceInternal(DirectoryEntry* parent, const CM::WPath& filePath)
	{
		ResourceEntry* newResource = cm_new<ResourceEntry>(filePath, toWString(PathUtil::getFilename(filePath)), parent);
		parent->mChildren.push_back(newResource);

		reimportResourceInternal(newResource);

		if(!onEntryAdded.empty())
			onEntryAdded(filePath);

		return newResource;
	}

	ProjectLibrary::DirectoryEntry* ProjectLibrary::addDirectoryInternal(DirectoryEntry* parent, const CM::WPath& dirPath)
	{
		DirectoryEntry* newEntry = cm_new<DirectoryEntry>(dirPath, toWString(PathUtil::getFilename(dirPath)), parent);
		parent->mChildren.push_back(newEntry);

		if(!onEntryAdded.empty())
			onEntryAdded(dirPath);

		return newEntry;
	}

	void ProjectLibrary::deleteResourceInternal(ResourceEntry* resource)
	{
		ResourceManifestPtr manifest = gResources().getResourceManifest();

		if(resource->meta != nullptr)
		{
			if(manifest->uuidExists(resource->meta->getUUID()))
			{
				const WString& path = manifest->uuidToFilePath(resource->meta->getUUID());
				if(FileSystem::isFile(path))
					FileSystem::remove(path);

				manifest->unregisterResource(resource->meta->getUUID());
			}
		}

		DirectoryEntry* parent = resource->parent;
		auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(), 
			[&] (const LibraryEntry* entry) { return entry == resource; });

		parent->mChildren.erase(findIter);

		if(!onEntryRemoved.empty())
			onEntryRemoved(resource->path);

		cm_delete(resource);
	}

	void ProjectLibrary::deleteDirectoryInternal(DirectoryEntry* directory)
	{
		if(directory == mRootEntry)
			mRootEntry = nullptr;

		for(auto& child : directory->mChildren)
		{
			if(child->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(child));
			else
				deleteResourceInternal(static_cast<ResourceEntry*>(child));
		}

		DirectoryEntry* parent = directory->parent;
		auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(), 
			[&] (const LibraryEntry* entry) { return entry == directory; });

		parent->mChildren.erase(findIter);

		if(!onEntryRemoved.empty())
			onEntryRemoved(directory->path);

		cm_delete(directory);
	}

	void ProjectLibrary::reimportResourceInternal(ResourceEntry* resource)
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
				fs.encode(resource->meta.get(), toWString(metaPath));
			}

			WPath internalResourcesPath = toPath(EditorApplication::instance().getActiveProjectPath()) / toPath(INTERNAL_RESOURCES_DIR);
			if(!FileSystem::isDirectory(internalResourcesPath))
				FileSystem::createDir(internalResourcesPath);

			internalResourcesPath /= toPath(toWString(importedResource.getUUID()) + L".asset");

			gResources().save(importedResource, toWString(internalResourcesPath), true);

			ResourceManifestPtr manifest = gResources().getResourceManifest();
			manifest->registerResource(importedResource.getUUID(), toWString(internalResourcesPath));

			resource->lastUpdateTime = std::time(nullptr);
		}
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

	ProjectLibrary::LibraryEntry* ProjectLibrary::findEntry(const CM::WPath& fullPath) const
	{
		Vector<WString>::type pathElems = PathUtil::split(toWString(fullPath));
		Vector<WString>::type rootElems = PathUtil::split(toWString(mRootEntry->path));

		auto pathIter = pathElems.begin();
		auto rootIter = rootElems.begin();

		while(pathIter != pathElems.end() && rootIter != rootElems.end() && PathUtil::comparePathElements(*pathIter, *rootIter))
		{
			++pathIter;
			++rootIter;
		}

		if(pathIter == pathElems.begin()) // Not a single entry matches Resources path
			return nullptr;

		--pathIter;
		Stack<LibraryEntry*>::type todo;
		todo.push(mRootEntry);

		while(!todo.empty())
		{
			LibraryEntry* current = todo.top();
			todo.pop();

			if(PathUtil::comparePathElements(*pathIter, current->elementName))
			{
				++pathIter;

				if(pathIter == pathElems.end())
					return current;

				while(!todo.empty())
					todo.pop();

				if(current->type == LibraryEntryType::Directory)
				{
					DirectoryEntry* dirEntry = static_cast<DirectoryEntry*>(current);
					for(auto& child : dirEntry->mChildren)
						todo.push(child);
				}
			}
		}

		return nullptr;
	}

	void ProjectLibrary::moveEntry(const CM::WPath& oldPath, const CM::WPath& newPath)
	{
		if(FileSystem::isFile(oldPath) || FileSystem::isDirectory(oldPath))
			FileSystem::move(oldPath, newPath);

		WPath oldMetaPath = getMetaPath(oldPath);
		WPath newMetaPath = getMetaPath(newPath);

		LibraryEntry* oldEntry = findEntry(oldPath);
		if(oldEntry != nullptr) // Moving from the Resources folder
		{
			// Moved outside of Resources, delete entry & meta file
			if(!PathUtil::includes(toWString(newPath), EditorApplication::instance().getResourcesFolderPath()))
			{
				if(oldEntry->type == LibraryEntryType::File)
				{
					deleteResourceInternal(static_cast<ResourceEntry*>(oldEntry));

					if(FileSystem::isFile(oldMetaPath))
						FileSystem::remove(oldMetaPath);
				}
				else if(oldEntry->type == LibraryEntryType::Directory)
					deleteDirectoryInternal(static_cast<DirectoryEntry*>(oldEntry));
			}
			else // Just moving internally
			{
				if(FileSystem::isFile(oldMetaPath))
					FileSystem::move(oldMetaPath, newMetaPath);

				DirectoryEntry* parent = oldEntry->parent;
				auto findIter = std::find(parent->mChildren.begin(), parent->mChildren.end(), oldEntry);
				if(findIter != parent->mChildren.end())
					parent->mChildren.erase(findIter);

				WPath parentPath = PathUtil::parentPath(newPath);

				DirectoryEntry* newEntryParent = nullptr;
				LibraryEntry* newEntryParentLib = findEntry(parentPath);
				if(newEntryParentLib != nullptr)
				{
					assert(newEntryParentLib->type == LibraryEntryType::Directory);
					newEntryParent = static_cast<DirectoryEntry*>(newEntryParentLib);
				}

				DirectoryEntry* newHierarchyParent = nullptr;
				if(newEntryParent == nullptr) // New path parent doesn't exist, so we need to create the hierarchy
					createInternalParentHierarchy(newPath, &newHierarchyParent, &newEntryParent);

				newEntryParent->mChildren.push_back(oldEntry);
				oldEntry->parent = newEntryParent;
				oldEntry->path = newPath;
				oldEntry->elementName = toWString(PathUtil::getFilename(newPath));

				if(!onEntryRemoved.empty())
					onEntryRemoved(oldPath);

				if(!onEntryAdded.empty())
					onEntryAdded(newPath);

				if(newHierarchyParent != nullptr)
					checkForModifications(toWString(newHierarchyParent->path));
			}
		}
		else // Moving from outside of the Resources folder (likely adding a new resource)
		{
			checkForModifications(toWString(newPath));
		}
	}

	void ProjectLibrary::deleteEntry(const CM::WPath& path)
	{
		if(FileSystem::isFile(path))
			FileSystem::remove(path);

		LibraryEntry* entry = findEntry(path);
		if(entry != nullptr)
		{
			if(entry->type == LibraryEntryType::File)
			{
				deleteResourceInternal(static_cast<ResourceEntry*>(entry));

				WPath metaPath = getMetaPath(path);
				if(FileSystem::isFile(metaPath))
					FileSystem::remove(metaPath);
			}
			else if(entry->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(entry));
		}
	}

	void ProjectLibrary::createInternalParentHierarchy(const CM::WPath& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf)
	{
		WPath parentPath = fullPath;

		DirectoryEntry* newEntryParent = nullptr;
		Stack<WPath>::type parentPaths;
		do 
		{
			WPath newParentPath = PathUtil::parentPath(parentPath);

			if(newParentPath == parentPath)
				break;

			LibraryEntry* newEntryParentLib = findEntry(newParentPath);
			if(newEntryParentLib != nullptr)
			{
				assert(newEntryParentLib->type == LibraryEntryType::Directory);
				newEntryParent = static_cast<DirectoryEntry*>(newEntryParentLib);

				break;
			}

			parentPaths.push(newParentPath);
			parentPath = newParentPath;

		} while (true);

		assert(newEntryParent != nullptr); // Must exist

		if(newHierarchyRoot != nullptr)
			*newHierarchyRoot = newEntryParent;

		while(!parentPaths.empty())
		{
			WPath curPath = parentPaths.top();
			parentPaths.pop();

			newEntryParent = addDirectoryInternal(newEntryParent, curPath);
		}

		if(newHierarchyLeaf != nullptr)
			*newHierarchyLeaf = newEntryParent;
	}

	WPath ProjectLibrary::getMetaPath(const CM::WPath& path) const
	{
		WPath metaPath = toPath(toWString(path) + L".meta");

		return metaPath;
	}

	bool ProjectLibrary::isMeta(const WPath& fullPath) const
	{
		return PathUtil::getExtension(fullPath) == toPath(L".meta");
	}

	void ProjectLibrary::onMonitorFileModified(const WString& path)
	{
		if(!isMeta(toPath(path)))
			checkForModifications(path);
		else
		{
			WPath resourcePath = toPath(path);
			PathUtil::replaceExtension(resourcePath, L"");

			checkForModifications(toWString(resourcePath));
		}
	}
}