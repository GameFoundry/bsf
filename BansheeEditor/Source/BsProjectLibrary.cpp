#include "BsProjectLibrary.h"
#include "BsFileSystem.h"
#include "BsException.h"
#include "BsResources.h"
#include "BsResourceManifest.h"
#include "BsImporter.h"
#include "BsProjectResourceMeta.h"
#include "BsResources.h"
#include "BsImporter.h"
#include "BsImportOptions.h"
#include "BsFileSerializer.h"
#include "BsDebug.h"
#include "BsProjectLibraryEntries.h"
#include "BsResource.h"
#include "BsResourceImporter.h"
#include "BsShader.h"
#include <regex>

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
		: lastUpdateTime(0)
	{ }

	ProjectLibrary::ResourceEntry::ResourceEntry(const Path& path, const WString& name, DirectoryEntry* parent)
		: LibraryEntry(path, name, parent, LibraryEntryType::File), lastUpdateTime(0)
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

		load();

		if(mResourceManifest == nullptr)
			mResourceManifest = ResourceManifest::create("ProjectLibrary");

		gResources().registerResourceManifest(mResourceManifest);
	}

	ProjectLibrary::~ProjectLibrary()
	{
		save();

		if(mRootEntry != nullptr)
			deleteDirectoryInternal(mRootEntry);
	}

	void ProjectLibrary::update()
	{
		while (!mReimportQueue.empty())
		{
			Path toReimport = *mReimportQueue.begin();
			LibraryEntry* entry = findEntry(toReimport);
			if (entry != nullptr && entry->type == LibraryEntryType::File)
			{
				ResourceEntry* resEntry = static_cast<ResourceEntry*>(entry);
				reimportResourceInternal(resEntry, resEntry->meta->getImportOptions(), true);
				queueDependantForReimport(resEntry);
			}

			mReimportQueue.erase(mReimportQueue.begin());
		}
	}

	void ProjectLibrary::checkForModifications(const Path& fullPath)
	{
		Vector<Path> dirtyResources;
		checkForModifications(fullPath, true, dirtyResources);
	}

	void ProjectLibrary::checkForModifications(const Path& fullPath, bool import, Vector<Path>& dirtyResources)
	{
		if (!mResourcesFolder.includes(fullPath))
			return; // Folder not part of our resources path, so no modifications

		if(mRootEntry == nullptr)
		{
			mRootEntry = bs_new<DirectoryEntry>(mResourcesFolder, mResourcesFolder.getWTail(), nullptr);
		}

		Path pathToSearch = fullPath;
		LibraryEntry* entry = findEntry(pathToSearch);
		if(entry == nullptr) // File could be new, try to find parent directory entry
		{
			if (isMeta(pathToSearch))
			{
				Path sourceFilePath = pathToSearch;
				sourceFilePath.setExtension(L"");

				if (!FileSystem::isFile(sourceFilePath))
				{
					LOGWRN("Found a .meta file without a corresponding resource. Deleting.");

					FileSystem::remove(pathToSearch);
				}
			}
			else
			{
				Path parentDirPath = pathToSearch.getParent();
				entry = findEntry(parentDirPath);

				// Cannot find parent directory. Create the needed hierarchy.
				DirectoryEntry* entryParent = nullptr;
				DirectoryEntry* newHierarchyParent = nullptr;
				if (entry == nullptr)
					createInternalParentHierarchy(pathToSearch, &newHierarchyParent, &entryParent);
				else
					entryParent = static_cast<DirectoryEntry*>(entry);

				if (FileSystem::isFile(pathToSearch))
				{
					if (import)
						addResourceInternal(entryParent, pathToSearch);

					dirtyResources.push_back(pathToSearch);
				}
				else if (FileSystem::isDirectory(pathToSearch))
				{
					addDirectoryInternal(entryParent, pathToSearch);

					checkForModifications(pathToSearch);
				}
			}
		}
		else if(entry->type == LibraryEntryType::File)
		{
			if(FileSystem::isFile(entry->path))
			{
				ResourceEntry* resEntry = static_cast<ResourceEntry*>(entry);

				if (import)
				{
					reimportResourceInternal(resEntry);
					queueDependantForReimport(resEntry);
				}

				if (!isUpToDate(resEntry))
					dirtyResources.push_back(entry->path);
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
				Stack<DirectoryEntry*> todo;
				todo.push(static_cast<DirectoryEntry*>(entry));

				Vector<Path> childFiles;
				Vector<Path> childDirectories;
				Vector<bool> existingEntries;
				Vector<LibraryEntry*> toDelete;

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
								if (import)
								{
									reimportResourceInternal(existingEntry);
									queueDependantForReimport(existingEntry);
								}

								if (!isUpToDate(existingEntry))
									dirtyResources.push_back(existingEntry->path);
							}
							else
							{
								if (import)
									addResourceInternal(currentDir, filePath);

								dirtyResources.push_back(filePath);
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

	ProjectLibrary::ResourceEntry* ProjectLibrary::addResourceInternal(DirectoryEntry* parent, const Path& filePath, 
		const ImportOptionsPtr& importOptions, bool forceReimport)
	{
		ResourceEntry* newResource = bs_new<ResourceEntry>(filePath, filePath.getWTail(), parent);
		parent->mChildren.push_back(newResource);

		reimportResourceInternal(newResource, importOptions, forceReimport);
		doOnEntryAdded(newResource);

		return newResource;
	}

	ProjectLibrary::DirectoryEntry* ProjectLibrary::addDirectoryInternal(DirectoryEntry* parent, const Path& dirPath)
	{
		DirectoryEntry* newEntry = bs_new<DirectoryEntry>(dirPath, dirPath.getWTail(), parent);
		parent->mChildren.push_back(newEntry);

		doOnEntryAdded(newEntry);
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

		doOnEntryRemoved(resource);
		bs_delete(resource);
	}

	void ProjectLibrary::deleteDirectoryInternal(DirectoryEntry* directory)
	{
		if(directory == mRootEntry)
			mRootEntry = nullptr;

		Vector<LibraryEntry*> childrenToDestroy = directory->mChildren;
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

		doOnEntryRemoved(directory);
		bs_delete(directory);
	}

	void ProjectLibrary::reimportResourceInternal(ResourceEntry* resource, const ImportOptionsPtr& importOptions, bool forceReimport)
	{
		WString ext = resource->path.getWExtension();
		Path metaPath = resource->path;
		metaPath.setFilename(metaPath.getWFilename() + L".meta");

		ext = ext.substr(1, ext.size() - 1); // Remove the .
		if (!Importer::instance().supportsFileType(ext))
			return;

		if(resource->meta == nullptr)
		{
			if(FileSystem::isFile(metaPath))
			{
				FileDecoder fs(metaPath);
				std::shared_ptr<IReflectable> loadedMeta = fs.decode();

				if(loadedMeta != nullptr && loadedMeta->isDerivedFrom(ProjectResourceMeta::getRTTIStatic()))
				{
					ProjectResourceMetaPtr resourceMeta = std::static_pointer_cast<ProjectResourceMeta>(loadedMeta);
					resource->meta = resourceMeta;
				}
			}
		}

		if (!isUpToDate(resource) || forceReimport)
		{
			ImportOptionsPtr curImportOptions = nullptr;

			if (importOptions == nullptr)
			{
				if (resource->meta != nullptr)
					curImportOptions = resource->meta->getImportOptions();
				else
					curImportOptions = Importer::instance().createImportOptions(resource->path);
			}
			else
				curImportOptions = importOptions;

			HResource importedResource;
			if(resource->meta == nullptr)
			{
				importedResource = Importer::instance().import(resource->path, curImportOptions);

				ResourceMetaDataPtr subMeta = importedResource->getMetaData();
				UINT32 typeId = importedResource->getTypeId();

				resource->meta = ProjectResourceMeta::create(importedResource.getUUID(), typeId, subMeta, curImportOptions);
				FileEncoder fs(metaPath);
				fs.encode(resource->meta.get());
			}
			else
			{
				removeDependencies(resource);

				importedResource = HResource(resource->meta->getUUID());
				Importer::instance().reimport(importedResource, resource->path, curImportOptions);
			}

			addDependencies(resource);

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

	Vector<ProjectLibrary::LibraryEntry*> ProjectLibrary::search(const WString& pattern)
	{
		return search(pattern, {});
	}

	Vector<ProjectLibrary::LibraryEntry*> ProjectLibrary::search(const WString& pattern, const Vector<UINT32>& typeIds)
	{
		Vector<LibraryEntry*> foundEntries;

		std::wregex escape(L"[.^$|()\\[\\]{}*+?\\\\]");
		WString replace(L"\\\\&");
		WString escapedPattern = std::regex_replace(pattern, escape, replace, std::regex_constants::match_default | std::regex_constants::format_sed);

		std::wregex wildcard(L"\\\\\\*");
		WString wildcardReplace(L".*");
		WString searchPattern = std::regex_replace(escapedPattern, wildcard, L".*");

		std::wregex searchRegex(searchPattern, std::regex_constants::ECMAScript | std::regex_constants::icase);

		Stack<DirectoryEntry*> todo;
		todo.push(mRootEntry);
		while (!todo.empty())
		{
			DirectoryEntry* dirEntry = todo.top();
			todo.pop();

			for (auto& child : dirEntry->mChildren)
			{
				if (std::regex_match(child->elementName, searchRegex))
				{
					if (typeIds.size() == 0)
						foundEntries.push_back(child);
					else
					{
						if (child->type == LibraryEntryType::File)
						{
							ResourceEntry* childResEntry = static_cast<ResourceEntry*>(child);
							for (auto& typeId : typeIds)
							{
								if (childResEntry->meta->getTypeID() == typeId)
								{
									foundEntries.push_back(child);
									break;
								}
							}
						}
					}
				}

				if (child->type == LibraryEntryType::Directory)
				{
					DirectoryEntry* childDirEntry = static_cast<DirectoryEntry*>(child);
					todo.push(childDirEntry);
				}
			}
		}

		std::sort(foundEntries.begin(), foundEntries.end(), 
			[&](const LibraryEntry* a, const LibraryEntry* b) 
		{ 
			return a->elementName.compare(b->elementName) < 0;
		});

		return foundEntries;
	}

	ProjectLibrary::LibraryEntry* ProjectLibrary::findEntry(const Path& path) const
	{
		Path fullPath = path;
		if (fullPath.isAbsolute())
		{
			if (!mResourcesFolder.includes(fullPath))
				return nullptr;
		}
		else
			fullPath.makeAbsolute(mResourcesFolder);

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

			if (current->type == LibraryEntryType::Directory)
			{
				DirectoryEntry* dirEntry = static_cast<DirectoryEntry*>(current);
				current = nullptr;
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
			else
				break;
		}

		return nullptr;
	}

	ProjectResourceMetaPtr ProjectLibrary::findResourceMeta(const String& uuid) const
	{
		if (mResourceManifest == nullptr)
			return nullptr;

		Path filePath;
		if (!mResourceManifest->uuidToFilePath(uuid, filePath))
			return nullptr;

		LibraryEntry* libEntry = findEntry(filePath);
		if (libEntry == nullptr || libEntry->type != LibraryEntryType::File)
			return nullptr;

		ResourceEntry* resEntry = static_cast<ResourceEntry*>(libEntry);
		return resEntry->meta;
	}

	Path ProjectLibrary::uuidToPath(const String& uuid) const
	{
		if (mResourceManifest == nullptr)
			return Path();

		Path filePath;
		if (!mResourceManifest->uuidToFilePath(uuid, filePath))
			return Path();

		return filePath;
	}

	void ProjectLibrary::createEntry(const HResource& resource, const Path& path)
	{
		if (resource == nullptr)
			return;

		Path assetPath = path;
		assetPath.setExtension(assetPath.getWExtension() + L"." + ResourceImporter::DEFAULT_EXTENSION);

		LibraryEntry* existingEntry = findEntry(assetPath);
		if (existingEntry != nullptr)
			BS_EXCEPT(InvalidParametersException, "Resource already exists at the specified path: " + assetPath.toString());

		Resources::instance().save(resource, assetPath, false);
		checkForModifications(assetPath);
	}

	void ProjectLibrary::saveEntry(const HResource& resource)
	{
		if (resource == nullptr)
			return;

		Path filePath;
		if (!mResourceManifest->uuidToFilePath(resource.getUUID(), filePath))
			return;

		Resources::instance().save(resource, filePath, false);
	}

	void ProjectLibrary::createFolderEntry(const Path& path)
	{
		Path fullPath = path;
		if (fullPath.isAbsolute())
		{
			if (!mResourcesFolder.includes(fullPath))
				return;
		}
		else
			fullPath.makeAbsolute(mResourcesFolder);

		if (FileSystem::isDirectory(fullPath))
			return; // Already exists

		FileSystem::createDir(fullPath);
		Path parentPath = fullPath.getParent();

		DirectoryEntry* newEntryParent = nullptr;
		LibraryEntry* newEntryParentLib = findEntry(parentPath);
		if (newEntryParentLib != nullptr)
		{
			assert(newEntryParentLib->type == LibraryEntryType::Directory);
			newEntryParent = static_cast<DirectoryEntry*>(newEntryParentLib);
		}

		DirectoryEntry* newHierarchyParent = nullptr;
		if (newEntryParent == nullptr) // New path parent doesn't exist, so we need to create the hierarchy
			createInternalParentHierarchy(fullPath, &newHierarchyParent, &newEntryParent);

		addDirectoryInternal(newEntryParent, fullPath);
	}

	void ProjectLibrary::moveEntry(const Path& oldPath, const Path& newPath, bool overwrite)
	{
		Path oldFullPath = oldPath;
		if (!oldFullPath.isAbsolute())
			oldFullPath.makeAbsolute(mResourcesFolder);

		Path newFullPath = newPath;
		if (!newFullPath.isAbsolute())
			newFullPath.makeAbsolute(mResourcesFolder);

		if(FileSystem::isFile(oldFullPath) || FileSystem::isDirectory(oldFullPath))
			FileSystem::move(oldFullPath, newFullPath, overwrite);

		Path oldMetaPath = getMetaPath(oldFullPath);
		Path newMetaPath = getMetaPath(newFullPath);

		LibraryEntry* oldEntry = findEntry(oldFullPath);
		if(oldEntry != nullptr) // Moving from the Resources folder
		{
			// Moved outside of Resources, delete entry & meta file
			if (!mResourcesFolder.includes(newFullPath))
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
				doOnEntryRemoved(oldEntry);

				if(FileSystem::isFile(oldMetaPath))
					FileSystem::move(oldMetaPath, newMetaPath);

				DirectoryEntry* parent = oldEntry->parent;
				auto findIter = std::find(parent->mChildren.begin(), parent->mChildren.end(), oldEntry);
				if(findIter != parent->mChildren.end())
					parent->mChildren.erase(findIter);

				Path parentPath = newFullPath.getParent();

				DirectoryEntry* newEntryParent = nullptr;
				LibraryEntry* newEntryParentLib = findEntry(parentPath);
				if(newEntryParentLib != nullptr)
				{
					assert(newEntryParentLib->type == LibraryEntryType::Directory);
					newEntryParent = static_cast<DirectoryEntry*>(newEntryParentLib);
				}

				DirectoryEntry* newHierarchyParent = nullptr;
				if(newEntryParent == nullptr) // New path parent doesn't exist, so we need to create the hierarchy
					createInternalParentHierarchy(newFullPath, &newHierarchyParent, &newEntryParent);

				newEntryParent->mChildren.push_back(oldEntry);
				oldEntry->parent = newEntryParent;
				oldEntry->path = newFullPath;
				oldEntry->elementName = newFullPath.getWTail();

				if(oldEntry->type == LibraryEntryType::Directory) // Update child paths
				{
					Stack<LibraryEntry*> todo;
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

				doOnEntryAdded(oldEntry);
			}
		}
		else // Moving from outside of the Resources folder (likely adding a new resource)
		{
			checkForModifications(newFullPath);
		}
	}

	void ProjectLibrary::copyEntry(const Path& oldPath, const Path& newPath, bool overwrite)
	{
		Path oldFullPath = oldPath;
		if (!oldFullPath.isAbsolute())
			oldFullPath.makeAbsolute(mResourcesFolder);

		Path newFullPath = newPath;
		if (!newFullPath.isAbsolute())
			newFullPath.makeAbsolute(mResourcesFolder);

		if (!FileSystem::exists(oldFullPath))
			return;

		FileSystem::copy(oldFullPath, newFullPath, overwrite);

		// Copying a file/folder outside of the Resources folder, no special handling needed
		if (!mResourcesFolder.includes(newFullPath))
			return;

		Path parentPath = newFullPath.getParent();

		DirectoryEntry* newEntryParent = nullptr;
		LibraryEntry* newEntryParentLib = findEntry(parentPath);
		if (newEntryParentLib != nullptr)
		{
			assert(newEntryParentLib->type == LibraryEntryType::Directory);
			newEntryParent = static_cast<DirectoryEntry*>(newEntryParentLib);
		}

		// If the source is outside of Resources folder, just plain import the copy
		LibraryEntry* oldEntry = findEntry(oldFullPath);
		if (oldEntry == nullptr)
		{
			checkForModifications(newFullPath);
			return;
		}

		// Both source and destination are within Resources folder, need to preserve import options on the copies
		LibraryEntry* newEntry = nullptr;
		if (FileSystem::isFile(newFullPath))
		{
			assert(oldEntry->type == LibraryEntryType::File);
			ResourceEntry* oldResEntry = static_cast<ResourceEntry*>(oldEntry);

			newEntry = addResourceInternal(newEntryParent, newFullPath, oldResEntry->meta->getImportOptions(), true);
		}
		else
		{
			assert(oldEntry->type == LibraryEntryType::File);
			DirectoryEntry* oldDirEntry = static_cast<DirectoryEntry*>(oldEntry);

			DirectoryEntry* newDirEntry = addDirectoryInternal(newEntryParent, newFullPath);
			newEntry = newDirEntry;

			Stack<std::tuple<DirectoryEntry*, DirectoryEntry*>> todo;
			todo.push(std::make_tuple(oldDirEntry, newDirEntry));

			while (!todo.empty())
			{
				auto current = todo.top();
				todo.pop();

				DirectoryEntry* sourceDir = std::get<0>(current);
				DirectoryEntry* destDir = std::get<1>(current);

				for (auto& child : sourceDir->mChildren)
				{
					Path childDestPath = destDir->path;
					childDestPath.append(child->path.getWTail());

					if (child->type == LibraryEntryType::File)
					{
						ResourceEntry* childResEntry = static_cast<ResourceEntry*>(child);

						addResourceInternal(destDir, childDestPath, childResEntry->meta->getImportOptions(), true);
					}
					else // Directory
					{
						DirectoryEntry* childSourceDirEntry = static_cast<DirectoryEntry*>(child);
						DirectoryEntry* childDestDirEntry = addDirectoryInternal(destDir, childDestPath);

						todo.push(std::make_tuple(childSourceDirEntry, childSourceDirEntry));
					}
				}
			}
		}
	}

	void ProjectLibrary::deleteEntry(const Path& path)
	{
		Path fullPath = path;
		if (!fullPath.isAbsolute())
			fullPath.makeAbsolute(mResourcesFolder);

		if(FileSystem::exists(fullPath))
			FileSystem::remove(fullPath);

		LibraryEntry* entry = findEntry(fullPath);
		if(entry != nullptr)
		{
			if(entry->type == LibraryEntryType::File)
			{
				deleteResourceInternal(static_cast<ResourceEntry*>(entry));

				Path metaPath = getMetaPath(fullPath);
				if(FileSystem::isFile(metaPath))
					FileSystem::remove(metaPath);
			}
			else if(entry->type == LibraryEntryType::Directory)
				deleteDirectoryInternal(static_cast<DirectoryEntry*>(entry));
		}
	}

	void ProjectLibrary::reimport(const Path& path, const ImportOptionsPtr& importOptions, bool forceReimport)
	{
		LibraryEntry* entry = findEntry(path);
		if (entry != nullptr)
		{
			if (entry->type == LibraryEntryType::File)
			{
				ResourceEntry* resEntry = static_cast<ResourceEntry*>(entry);
				reimportResourceInternal(resEntry, importOptions, forceReimport);
				queueDependantForReimport(resEntry);
			}
		}
	}

	HResource ProjectLibrary::load(const Path& path)
	{
		LibraryEntry* entry = findEntry(path);

		if (entry == nullptr || entry->type == LibraryEntryType::Directory)
			return HResource();

		ResourceEntry* resEntry = static_cast<ResourceEntry*>(entry);
		String resUUID = resEntry->meta->getUUID();

		return Resources::instance().loadFromUUID(resUUID);
	}

	void ProjectLibrary::createInternalParentHierarchy(const Path& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf)
	{
		Path parentPath = fullPath;

		DirectoryEntry* newEntryParent = nullptr;
		Stack<Path> parentPaths;
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

	void ProjectLibrary::save()
	{
		std::shared_ptr<ProjectLibraryEntries> libEntries = ProjectLibraryEntries::create(*mRootEntry);

		Path libraryEntriesPath = mProjectFolder;
		libraryEntriesPath.append(INTERNAL_RESOURCES_DIR);
		libraryEntriesPath.append(LIBRARY_ENTRIES_FILENAME);

		FileEncoder fs(libraryEntriesPath);
		fs.encode(libEntries.get());

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

		mRootEntry = bs_new<DirectoryEntry>(mResourcesFolder, mResourcesFolder.getWTail(), nullptr);

		Path libraryEntriesPath = mProjectFolder;
		libraryEntriesPath.append(INTERNAL_RESOURCES_DIR);
		libraryEntriesPath.append(LIBRARY_ENTRIES_FILENAME);

		if(FileSystem::exists(libraryEntriesPath))
		{
			FileDecoder fs(libraryEntriesPath);
			std::shared_ptr<ProjectLibraryEntries> libEntries = std::static_pointer_cast<ProjectLibraryEntries>(fs.decode());

			*mRootEntry = libEntries->getRootEntry();
			for(auto& child : mRootEntry->mChildren)
				child->parent = mRootEntry;

			mRootEntry->parent = nullptr;
		}

		// Load all meta files
		Stack<DirectoryEntry*> todo;
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
					bool doAddDependencies = true;

					if (FileSystem::isFile(resEntry->path))
					{
						if (resEntry->meta == nullptr)
						{
							Path metaPath = resEntry->path;
							metaPath.setFilename(metaPath.getWFilename() + L".meta");

							if (FileSystem::isFile(metaPath))
							{
								FileDecoder fs(metaPath);
								std::shared_ptr<IReflectable> loadedMeta = fs.decode();

								if (loadedMeta != nullptr && loadedMeta->isDerivedFrom(ProjectResourceMeta::getRTTIStatic()))
								{
									ProjectResourceMetaPtr resourceMeta = std::static_pointer_cast<ProjectResourceMeta>(loadedMeta);
									resEntry->meta = resourceMeta;
								}
							}
							else
							{
								LOGWRN("Missing meta file: " + metaPath.toString() + ". Triggering reimport.");
								reimportResourceInternal(resEntry);
								doAddDependencies = false;
							}
						}
					}

					if (doAddDependencies)
						addDependencies(resEntry);
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

	void ProjectLibrary::doOnEntryRemoved(const LibraryEntry* entry)
	{
		if (!onEntryRemoved.empty())
			onEntryRemoved(entry->path);

		if (entry->type == LibraryEntryType::File)
		{
			const ResourceEntry* resEntry = static_cast<const ResourceEntry*>(entry);
			queueDependantForReimport(resEntry);
			removeDependencies(resEntry);
		}
	}

	void ProjectLibrary::doOnEntryAdded(const LibraryEntry* entry)
	{
		if (!onEntryAdded.empty())
			onEntryAdded(entry->path);

		if (entry->type == LibraryEntryType::File)
		{
			const ResourceEntry* resEntry = static_cast<const ResourceEntry*>(entry);
			queueDependantForReimport(resEntry);
		}
	}

	Vector<Path> ProjectLibrary::getImportDependencies(const ResourceEntry* entry)
	{
		Vector<Path> output;

		if (entry->meta == nullptr)
			return output;

		if (entry->meta->getTypeID() == TID_Shader)
		{
			SPtr<ShaderMetaData> metaData = std::static_pointer_cast<ShaderMetaData>(entry->meta->getResourceMetaData());

			for (auto& include : metaData->includes)
				output.push_back(include);
		}

		return output;
	}

	void ProjectLibrary::addDependencies(const ResourceEntry* entry)
	{
		Vector<Path> dependencies = getImportDependencies(entry);
		for (auto& dependency : dependencies)
			mDependencies[dependency].push_back(entry->path);
	}

	void ProjectLibrary::removeDependencies(const ResourceEntry* entry)
	{
		Vector<Path> dependencies = getImportDependencies(entry);
		for (auto& dependency : dependencies)
		{
			Vector<Path>& curDependencies = mDependencies[dependency];
			auto iterRemove = std::remove_if(curDependencies.begin(), curDependencies.end(),
				[&](const Path& x)
			{
				return x == entry->path;
			});

			curDependencies.erase(iterRemove, curDependencies.end());
		}
	}

	void ProjectLibrary::queueDependantForReimport(const ResourceEntry* entry)
	{
		auto iterFind = mDependencies.find(entry->path);
		if (iterFind == mDependencies.end())
			return;

		for (auto& dependency : iterFind->second)
			mReimportQueue.insert(dependency);
	}
}