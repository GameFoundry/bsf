#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT ProjectLibrary : public Module<ProjectLibrary>
	{
	public:
		struct LibraryEntry;
		struct ResourceEntry;
		struct DirectoryEntry;

		enum class LibraryEntryType
		{
			File,
			Directory
		};

		struct LibraryEntry
		{
			LibraryEntry();
			LibraryEntry(const Path& path, const WString& name, DirectoryEntry* parent, LibraryEntryType type);

			LibraryEntryType type;
			Path path;
			WString elementName;

			DirectoryEntry* parent;
		};

		struct ResourceEntry : public LibraryEntry
		{
			ResourceEntry();
			ResourceEntry(const Path& path, const WString& name, DirectoryEntry* parent);

			ProjectResourceMetaPtr meta;
			std::time_t lastUpdateTime;
		};

		struct DirectoryEntry : public LibraryEntry
		{
			DirectoryEntry();
			DirectoryEntry(const Path& path, const WString& name, DirectoryEntry* parent);

			Vector<LibraryEntry*> mChildren;
		};

	public:
		ProjectLibrary(const Path& projectFolder);
		~ProjectLibrary();

		void update();
		void checkForModifications(const Path& fullPath);

		const LibraryEntry* getRootEntry() const { return mRootEntry; }
		LibraryEntry* findEntry(const Path& fullPath) const;

		Vector<LibraryEntry*> search(const WString& pattern);
		Vector<LibraryEntry*> search(const WString& pattern, const Vector<UINT32>& typeIds);

		ProjectResourceMetaPtr findResourceMeta(const String& uuid) const;
		Path uuidToPath(const String& uuid) const;

		void createEntry(const HResource& resource, const Path& path);
		void createFolderEntry(const Path& path);
		void saveEntry(const HResource& resource);
		void moveEntry(const Path& oldPath, const Path& newPath, bool overwrite = true);
		void copyEntry(const Path& oldPath, const Path& newPath, bool overwrite = true);
		void deleteEntry(const Path& path);
		void reimport(const Path& path, const ImportOptionsPtr& importOptions = nullptr, bool forceReimport = false);

		const Path& getResourcesFolder() const { return mResourcesFolder; }

		Event<void(const Path&)> onEntryRemoved;
		Event<void(const Path&)> onEntryAdded;
	private:
		static const Path RESOURCES_DIR;
		static const Path INTERNAL_RESOURCES_DIR;
		static const WString LIBRARY_ENTRIES_FILENAME;
		static const WString RESOURCE_MANIFEST_FILENAME;

		ResourceManifestPtr mResourceManifest;
		DirectoryEntry* mRootEntry;
		FolderMonitor* mMonitor;
		Path mProjectFolder;
		Path mResourcesFolder;

		UnorderedMap<Path, Vector<Path>> mDependencies;
		UnorderedSet<Path> mReimportQueue;

		void save();
		void load();

		ResourceEntry* addResourceInternal(DirectoryEntry* parent, const Path& filePath, const ImportOptionsPtr& importOptions = nullptr, bool forceReimport = false);
		DirectoryEntry* addDirectoryInternal(DirectoryEntry* parent, const Path& dirPath);

		void deleteResourceInternal(ResourceEntry* resource);
		void deleteDirectoryInternal(DirectoryEntry* directory);

		void reimportResourceInternal(ResourceEntry* resource, const ImportOptionsPtr& importOptions = nullptr, bool forceReimport = false);

		void createInternalParentHierarchy(const Path& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf);

		bool isUpToDate(ResourceEntry* resource) const;
		Path getMetaPath(const Path& path) const;
		bool isMeta(const Path& fullPath) const;

		void doOnEntryRemoved(const LibraryEntry* entry);
		void doOnEntryAdded(const LibraryEntry* entry);

		void onMonitorFileModified(const Path& path);

		Vector<Path> getImportDependencies(const ResourceEntry* entry);
		void addDependencies(const ResourceEntry* entry);
		void removeDependencies(const ResourceEntry* entry);
		void queueDependantForReimport(const ResourceEntry* entry);
	};
}