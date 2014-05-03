#include "BsProjectLibrary.h"
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
#include "BsProjectLibraryEntries.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const Path ProjectLibrary::RESOURCES_DIR = L"Resources\\";
	const Path ProjectLibrary::INTERNAL_RESOURCES_DIR = L"Internal\\Resources\\";
	const WString ProjectLibrary::LIBRARY_ENTRIES_FILENAME = L"ProjectLibrary.asset";
	const WString ProjectLibrary::RESOURCE_MANIFEST_FILENAME = L"ResourceManifest.asset";

	ProjectLibrary::LibraryEntry::LibraryEntry()
		:parent(nullptr), type(LibraryEntryType::Directory)
	{ }

	ProjectLibrary::LibraryEntry::LibraryEntry(const Path& path, const WString& name, DirectoryEntry* parent, LibraryEntryType type)
		:path(path), parent(parent), type(type), elementName(name)
	{ }

	ProjectLibrary::ResourceEntry::ResourceEntry()
		:lastUpdateTime(0)
	{ }

	ProjectLibrary::ResourceEntry::ResourceEntry(const Path& path, const WString& name, DirectoryEntry* parent)
		:LibraryEntry(path, name, parent, LibraryEntryType::File), lastUpdateTime(0)
	{ }

	ProjectLibrary::DirectoryEntry::DirectoryEntry()
	{ }

	ProjectLibrary::DirectoryEntry::DirectoryEntry(const Path& path, const WString& name, DirectoryEntry* parent)
		:LibraryEntry(path, name, parent, LibraryEntryType::Directory)
	{ }

	ProjectLibrary::ProjectLibrary(const Path& projectFolder)
		:mRootEntry(nullptr), mProjectFolder(projectFolder)
	{
		mResourcesFolder = mProjectFolder;
		mResourcesFolder.append(RESOURCES_DIR);
		mMonitor = cm_new<FolderMonitor>();

		FolderChange folderChanges = (FolderChange)((UINT32)FolderChange::FileName | (UINT32)FolderChange::DirName | 
				(UINT32)FolderChange::Creation | (UINT32)FolderChange::LastWrite);
		mMonitor->startMonitor(mResourcesFolder, true, folderChanges);

		mMonitor->onAdded.connect(std::bind(&ProjectLibrary::onMonitorFileModified, this, _1));
		mMonitor->onRemoved.connect(std::bind(&ProjectLibrary::onMonitorFileModified, this, _1));
		mMonitor->onModified.connect(std::bind(&ProjectLibrary::onMonitorFileModified, this, _1));

		load();

		if(mResourceManifest == nullptr)
			mResourceManifest = ResourceManifest::create("ProjectLibrary");

		gResources().registerResourceManifest(mResourceManifest);

		checkForModifications(mResourcesFolder);
	}

	ProjectLibrary::~ProjectLibrary()
	{
		save();

		mMonitor->stopMonitorAll();
		cm_delete(mMonitor);

		if(mRootEntry != nullptr)
			deleteDirectoryInternal(mRootEntry);
	}

	void ProjectLibrary::update()
	{
		mMonitor->_update();
	}

	void ProjectLibrary::checkForModifications(const Path& fullPath)
	{
		if (!mResourcesFolder.includes(fullPath))
			return; // Folder not part of our resources path, so no modifications

		if(mRootEntry == nullptr)
		{
			mRootEntry = cm_new<DirectoryEntry>(mResourcesFolder, mResourcesFolder.getWTail(), nullptr);
		}

		Path pathToSearch = fullPath;
		LibraryEntry* entry = findEntry(pathToSearch);
		if(entry == nullptr) // File could be new, try to find parent directory entry
		{
			Path parentDirPath = pathToSearch.getParent();
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
					checkForModifications(pathToSearch);
			}

			if(newHierarchyParent != nullptr)
				checkForModifications(newHierarchyParent->path);
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

				Vector<Path>::type childFiles;
				Vector<Path>::type childDirectories;
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
							Path sourceFilePath = filePath;
							sourceFilePath.setExtension(L"");

							if(!FileSystem::isFile(sourceFilePath))
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

	ProjectLibrary::ResourceEntry* ProjectLibrary::addResourceInternal(DirectoryEntry* parent, const Path& filePath)
	{
		ResourceEntry* newResource = cm_new<ResourceEntry>(filePath, filePath.getWTail(), parent);
		parent->mChildren.push_back(newResource);

		reimportResourceInternal(newResource);

		if(!onEntryAdded.empty())
			onEntryAdded(filePath);

		return newResource;
	}

	ProjectLibrary::DirectoryEntry* ProjectLibrary::addDirectoryInternal(DirectoryEntry* parent, const Path& dirPath)
	{
		DirectoryEntry* newEntry = cm_new<DirectoryEntry>(dirPath, dirPath.getWTail(), parent);
		parent->mChildren.push_back(newEntry);

		if(!onEntryAdded.empty())
			onEntryAdded(dirPath);

		return newEntry;
	}

	void ProjectLibrary::deleteResourceInternal(ResourceEntry* resource)
	{
		if(resource->meta != nullptr)
		{
			Path path;
			if(mResourceManifest->uuidToFilePath(resource->meta->getUUID(), path))
			{
				if(FileSystem::isFile(path))
					FileSystem::remove(path);

				mResourceManifest->unregisterResource(resource->meta->getUUID());
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

		Vector<LibraryEntry*>::type childrenToDestroy = directory->mChildren;
		for(auto& child : childrenToDestroy)
		{
			if(child->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(child));
			else
				deleteResourceInternal(static_cast<ResourceEntry*>(child));
		}

		DirectoryEntry* parent = directory->parent;
		if(parent != nullptr)
		{
			auto findIter = std::find_if(parent->mChildren.begin(), parent->mChildren.end(), 
				[&] (const LibraryEntry* entry) { return entry == directory; });

			parent->mChildren.erase(findIter);
		}

		if(!onEntryRemoved.empty())
			onEntryRemoved(directory->path);

		cm_delete(directory);
	}

	void ProjectLibrary::reimportResourceInternal(ResourceEntry* resource)
	{
		WString ext = resource->path.getWExtension();
		Path metaPath = resource->path;
		metaPath.setFilename(metaPath.getWFilename() + L".meta");

		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if(!Importer::instance().supportsFileType(ext))
			return;

		if(resource->meta == nullptr)
		{
			FileSerializer fs;
			if(FileSystem::isFile(metaPath))
			{
				std::shared_ptr<IReflectable> loadedMeta = fs.decode(metaPath);

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
				importOptions = Importer::instance().createImportOptions(resource->path);

			HResource importedResource;

			if(resource->meta == nullptr)
			{
				importedResource = Importer::instance().import(resource->path, importOptions);

				resource->meta = ResourceMeta::create(importedResource.getUUID(), importOptions);
				FileSerializer fs;
				fs.encode(resource->meta.get(), metaPath);
			}
			else
			{
				importedResource = HResource(resource->meta->getUUID());
				Importer::instance().reimport(importedResource, resource->path, importOptions);
			}

			Path internalResourcesPath = mProjectFolder;
			internalResourcesPath.append(INTERNAL_RESOURCES_DIR);

			if(!FileSystem::isDirectory(internalResourcesPath))
				FileSystem::createDir(internalResourcesPath);

			internalResourcesPath.setFilename(toWString(importedResource.getUUID()) + L".asset");

			gResources().save(importedResource, internalResourcesPath, true);
			gResources().unload(importedResource);

			mResourceManifest->registerResource(importedResource.getUUID(), internalResourcesPath);

			resource->lastUpdateTime = std::time(nullptr);
		}
	}

	bool ProjectLibrary::isUpToDate(ResourceEntry* resource) const
	{
		if(resource->meta == nullptr)
			return false;

		Path path;
		if(!mResourceManifest->uuidToFilePath(resource->meta->getUUID(), path))
			return false;

		if(!FileSystem::isFile(path))
			return false;

		std::time_t lastModifiedTime = FileSystem::getLastModifiedTime(path);
		return lastModifiedTime <= resource->lastUpdateTime;
	}

	ProjectLibrary::LibraryEntry* ProjectLibrary::findEntry(const Path& fullPath) const
	{
		if (!mRootEntry->path.includes(fullPath))
			return nullptr;

		Path relPath = fullPath.getRelative(mRootEntry->path);
		UINT32 numElems = relPath.getNumDirectories() + (relPath.isFile() ? 1 : 0);
		UINT32 idx = 0;

		LibraryEntry* current = mRootEntry;
		while (current != nullptr)
		{
			if (idx == numElems)
				return current;

			WString curElem;
			if (relPath.isFile() && idx == (numElems - 1))
				curElem = relPath.getWFilename();
			else
				curElem = relPath[idx];

			current = nullptr;
			if (current->type == LibraryEntryType::Directory)
			{
				DirectoryEntry* dirEntry = static_cast<DirectoryEntry*>(current);
				for (auto& child : dirEntry->mChildren)
				{
					if (Path::comparePathElem(curElem, child->elementName))
					{
						idx++;
						current = child;
						break;
					}
				}
			}
		}

		return nullptr;
	}

	void ProjectLibrary::moveEntry(const Path& oldPath, const Path& newPath)
	{
		if(FileSystem::isFile(oldPath) || FileSystem::isDirectory(oldPath))
			FileSystem::move(oldPath, newPath);

		Path oldMetaPath = getMetaPath(oldPath);
		Path newMetaPath = getMetaPath(newPath);

		LibraryEntry* oldEntry = findEntry(oldPath);
		if(oldEntry != nullptr) // Moving from the Resources folder
		{
			// Moved outside of Resources, delete entry & meta file
			if (!mResourcesFolder.includes(newPath))
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

				Path parentPath = newPath.getParent();

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
				oldEntry->elementName = newPath.getWTail();

				if(oldEntry->type == LibraryEntryType::Directory) // Update child paths
				{
					Stack<LibraryEntry*>::type todo;
					todo.push(oldEntry);

					while(!todo.empty())
					{
						LibraryEntry* curEntry = todo.top();
						todo.pop();

						DirectoryEntry* curDirEntry = static_cast<DirectoryEntry*>(curEntry);
						for(auto& child : curDirEntry->mChildren)
						{
							child->path = child->parent->path;
							child->path.append(child->elementName);

							if(child->type == LibraryEntryType::Directory)
								todo.push(child);
						}
					}
				}

				if(!onEntryRemoved.empty())
					onEntryRemoved(oldPath);

				if(!onEntryAdded.empty())
					onEntryAdded(newPath);

				if(newHierarchyParent != nullptr)
					checkForModifications(newHierarchyParent->path);
			}
		}
		else // Moving from outside of the Resources folder (likely adding a new resource)
		{
			checkForModifications(newPath);
		}
	}

	void ProjectLibrary::deleteEntry(const Path& path)
	{
		if(FileSystem::exists(path))
			FileSystem::remove(path);

		LibraryEntry* entry = findEntry(path);
		if(entry != nullptr)
		{
			if(entry->type == LibraryEntryType::File)
			{
				deleteResourceInternal(static_cast<ResourceEntry*>(entry));

				Path metaPath = getMetaPath(path);
				if(FileSystem::isFile(metaPath))
					FileSystem::remove(metaPath);
			}
			else if(entry->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(entry));
		}
	}

	void ProjectLibrary::createInternalParentHierarchy(const Path& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf)
	{
		Path parentPath = fullPath;

		DirectoryEntry* newEntryParent = nullptr;
		Stack<Path>::type parentPaths;
		do 
		{
			Path newParentPath = parentPath.getParent();

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
			Path curPath = parentPaths.top();
			parentPaths.pop();

			newEntryParent = addDirectoryInternal(newEntryParent, curPath);
		}

		if(newHierarchyLeaf != nullptr)
			*newHierarchyLeaf = newEntryParent;
	}

	Path ProjectLibrary::getMetaPath(const Path& path) const
	{
		Path metaPath = path;
		metaPath.setFilename(metaPath.getWFilename() + L".meta");

		return metaPath;
	}

	bool ProjectLibrary::isMeta(const Path& fullPath) const
	{
		return fullPath.getWExtension() == L".meta";
	}

	void ProjectLibrary::onMonitorFileModified(const Path& path)
	{
		if(!isMeta(path))
			checkForModifications(path);
		else
		{
			Path resourcePath = path;
			resourcePath.setExtension(L"");

			checkForModifications(resourcePath);
		}
	}

	void ProjectLibrary::save()
	{
		std::shared_ptr<ProjectLibraryEntries> libEntries = ProjectLibraryEntries::create(*mRootEntry);

		Path libraryEntriesPath = mProjectFolder;
		libraryEntriesPath.append(INTERNAL_RESOURCES_DIR);
		libraryEntriesPath.append(LIBRARY_ENTRIES_FILENAME);

		FileSerializer fs;
		fs.encode(libEntries.get(), libraryEntriesPath);

		Path resourceManifestPath = mProjectFolder;
		resourceManifestPath.append(INTERNAL_RESOURCES_DIR);
		resourceManifestPath.append(RESOURCE_MANIFEST_FILENAME);

		ResourceManifest::save(mResourceManifest, resourceManifestPath, mProjectFolder);
	}

	void ProjectLibrary::load()
	{
		if(mRootEntry != nullptr)
		{
			deleteDirectoryInternal(mRootEntry);
			mRootEntry = nullptr;
		}

		mRootEntry = cm_new<DirectoryEntry>(mResourcesFolder, mResourcesFolder.getWTail(), nullptr);

		Path libraryEntriesPath = mProjectFolder;
		libraryEntriesPath.append(INTERNAL_RESOURCES_DIR);
		libraryEntriesPath.append(LIBRARY_ENTRIES_FILENAME);

		if(FileSystem::exists(libraryEntriesPath))
		{
			FileSerializer fs;
			std::shared_ptr<ProjectLibraryEntries> libEntries = std::static_pointer_cast<ProjectLibraryEntries>(fs.decode(libraryEntriesPath));

			*mRootEntry = libEntries->getRootEntry();
			for(auto& child : mRootEntry->mChildren)
				child->parent = mRootEntry;

			mRootEntry->parent = nullptr;
		}

		// Load all meta files
		Stack<DirectoryEntry*>::type todo;
		todo.push(mRootEntry);

		while(!todo.empty())
		{
			DirectoryEntry* curDir = todo.top();
			todo.pop();

			for(auto& child : curDir->mChildren)
			{
				if(child->type == LibraryEntryType::File)
				{
					ResourceEntry* resEntry = static_cast<ResourceEntry*>(child);

					if(resEntry->meta == nullptr)
					{
						Path metaPath = resEntry->path;
						metaPath.setFilename(metaPath.getWFilename() + L".meta");

						FileSerializer fs;
						if(FileSystem::isFile(metaPath))
						{
							std::shared_ptr<IReflectable> loadedMeta = fs.decode(metaPath);

							if(loadedMeta != nullptr && loadedMeta->isDerivedFrom(ResourceMeta::getRTTIStatic()))
							{
								ResourceMetaPtr resourceMeta = std::static_pointer_cast<ResourceMeta>(loadedMeta);
								resEntry->meta = resourceMeta;
							}
						}
					}
				}
				else if(child->type == LibraryEntryType::Directory)
				{
					todo.push(static_cast<DirectoryEntry*>(child));
				}
			}
		}

		// Load resource manifest
		Path resourceManifestPath = mProjectFolder;
		resourceManifestPath.append(INTERNAL_RESOURCES_DIR);
		resourceManifestPath.append(RESOURCE_MANIFEST_FILENAME);

		if(FileSystem::exists(resourceManifestPath))
		{
			mResourceManifest = ResourceManifest::load(resourceManifestPath, mProjectFolder);
		}
	}
}