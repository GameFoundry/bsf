#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsPath.h"

namespace BansheeEngine
{
	class ProjectLibrary : public Module<ProjectLibrary>
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

			ResourceMetaPtr meta;
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

		ResourceMetaPtr findResourceMeta(const String& uuid) const;

		void moveEntry(const Path& oldPath, const Path& newPath);
		void deleteEntry(const Path& path);

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

		void save();
		void load();

		ResourceEntry* addResourceInternal(DirectoryEntry* parent, const Path& filePath);
		DirectoryEntry* addDirectoryInternal(DirectoryEntry* parent, const Path& dirPath);

		void deleteResourceInternal(ResourceEntry* resource);
		void deleteDirectoryInternal(DirectoryEntry* directory);

		void reimportResourceInternal(ResourceEntry* resource);

		void createInternalParentHierarchy(const Path& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf);

		bool isUpToDate(ResourceEntry* resource) const;
		Path getMetaPath(const Path& path) const;
		bool isMeta(const Path& fullPath) const;

		void onMonitorFileModified(const Path& path);
	};
}