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

	ProjectLibrary::LibraryEntry::LibraryEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent, LibraryEntryType type)
		:path(path), parent(parent), type(type), elementName(name)
	{ }

	ProjectLibrary::ResourceEntry::ResourceEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent)
		:LibraryEntry(path, name, parent, LibraryEntryType::File), lastUpdateTime(0)
	{ }

	ProjectLibrary::DirectoryEntry::DirectoryEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent)
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
		if(!Path::includes(fullPath, EditorApplication::instance().getResourcesFolderPath()))
			return; // Folder not part of our resources path, so no modifications

		if(mRootEntry == nullptr)
		{
			WString resPath = EditorApplication::instance().getResourcesFolderPath();
			mRootEntry = cm_new<DirectoryEntry>(resPath, Path::getFilename(resPath), nullptr);
		}

		WString pathToSearch = fullPath;
		LibraryEntry* entry = findEntry(pathToSearch);
		if(entry == nullptr) // File could be new, try to find parent directory entry
		{
			WString parentDirPath = Path::parentPath(pathToSearch);
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

				Vector<WString>::type childFiles;
				Vector<WString>::type childDirectories;
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
							WString sourceFilePath = filePath;
							Path::replaceExtension(sourceFilePath, L"");

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
								if(child->type == LibraryEntryType::File && Path::equals(child->path, filePath))
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
							if(child->type == LibraryEntryType::Directory && Path::equals(child->path, dirPath))
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

	ProjectLibrary::ResourceEntry* ProjectLibrary::addResourceInternal(DirectoryEntry* parent, const CM::WString& filePath)
	{
		ResourceEntry* newResource = cm_new<ResourceEntry>(filePath, Path::getFilename(filePath), parent);
		parent->mChildren.push_back(newResource);

		reimportResourceInternal(newResource);

		if(!onEntryAdded.empty())
			onEntryAdded(filePath);

		return newResource;
	}

	ProjectLibrary::DirectoryEntry* ProjectLibrary::addDirectoryInternal(DirectoryEntry* parent, const CM::WString& dirPath)
	{
		DirectoryEntry* newEntry = cm_new<DirectoryEntry>(dirPath, Path::getFilename(dirPath), parent);
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

		CM::Vector<LibraryEntry*>::type childrenToDestroy = directory->mChildren;
		for(auto& child : childrenToDestroy)
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
		WString ext = Path::getExtension(resource->path);
		WString metaPath = resource->path + L".meta";

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

			HResource importedResource = Importer::instance().import(resource->path, importOptions);

			if(resource->meta == nullptr)
			{
				resource->meta = ResourceMeta::create(importedResource.getUUID(), importOptions);
				FileSerializer fs;
				fs.encode(resource->meta.get(), metaPath);
			}

			WString internalResourcesPath = Path::combine(EditorApplication::instance().getActiveProjectPath(), INTERNAL_RESOURCES_DIR);
			if(!FileSystem::isDirectory(internalResourcesPath))
				FileSystem::createDir(internalResourcesPath);

			internalResourcesPath = Path::combine(internalResourcesPath, toWString(importedResource.getUUID()) + L".asset");

			gResources().save(importedResource, internalResourcesPath, true);

			ResourceManifestPtr manifest = gResources().getResourceManifest();
			manifest->registerResource(importedResource.getUUID(), internalResourcesPath);

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

	ProjectLibrary::LibraryEntry* ProjectLibrary::findEntry(const CM::WString& fullPath) const
	{
		Vector<WString>::type pathElems = Path::split(fullPath);
		Vector<WString>::type rootElems = Path::split(mRootEntry->path);

		auto pathIter = pathElems.begin();
		auto rootIter = rootElems.begin();

		while(pathIter != pathElems.end() && rootIter != rootElems.end() && Path::comparePathElements(*pathIter, *rootIter))
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

			if(Path::comparePathElements(*pathIter, current->elementName))
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

	void ProjectLibrary::moveEntry(const CM::WString& oldPath, const CM::WString& newPath)
	{
		if(FileSystem::isFile(oldPath) || FileSystem::isDirectory(oldPath))
			FileSystem::move(oldPath, newPath);

		WString oldMetaPath = getMetaPath(oldPath);
		WString newMetaPath = getMetaPath(newPath);

		LibraryEntry* oldEntry = findEntry(oldPath);
		if(oldEntry != nullptr) // Moving from the Resources folder
		{
			// Moved outside of Resources, delete entry & meta file
			if(!Path::includes(newPath, EditorApplication::instance().getResourcesFolderPath()))
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

				WString parentPath = Path::parentPath(newPath);

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
				oldEntry->elementName = Path::getFilename(newPath);

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
							child->path = Path::combine(child->parent->path, child->elementName);

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

	void ProjectLibrary::deleteEntry(const CM::WString& path)
	{
		if(FileSystem::isFile(path))
			FileSystem::remove(path);

		LibraryEntry* entry = findEntry(path);
		if(entry != nullptr)
		{
			if(entry->type == LibraryEntryType::File)
			{
				deleteResourceInternal(static_cast<ResourceEntry*>(entry));

				WString metaPath = getMetaPath(path);
				if(FileSystem::isFile(metaPath))
					FileSystem::remove(metaPath);
			}
			else if(entry->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(entry));
		}
	}

	void ProjectLibrary::createInternalParentHierarchy(const CM::WString& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf)
	{
		WString parentPath = fullPath;

		DirectoryEntry* newEntryParent = nullptr;
		Stack<WString>::type parentPaths;
		do 
		{
			WString newParentPath = Path::parentPath(parentPath);

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
			WString curPath = parentPaths.top();
			parentPaths.pop();

			newEntryParent = addDirectoryInternal(newEntryParent, curPath);
		}

		if(newHierarchyLeaf != nullptr)
			*newHierarchyLeaf = newEntryParent;
	}

	WString ProjectLibrary::getMetaPath(const CM::WString& path) const
	{
		WString metaPath = path + L".meta";

		return metaPath;
	}

	bool ProjectLibrary::isMeta(const WString& fullPath) const
	{
		return Path::getExtension(fullPath) == L".meta";
	}

	void ProjectLibrary::onMonitorFileModified(const WString& path)
	{
		if(!isMeta(path))
			checkForModifications(path);
		else
		{
			WString resourcePath = path;
			Path::replaceExtension(resourcePath, L"");

			checkForModifications(resourcePath);
		}
	}
}