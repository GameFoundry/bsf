#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

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
			LibraryEntry(const WString& path, const WString& name, DirectoryEntry* parent, LibraryEntryType type);

			LibraryEntryType type;
			WString path;
			WString elementName;

			DirectoryEntry* parent;
		};

		struct ResourceEntry : public LibraryEntry
		{
			ResourceEntry();
			ResourceEntry(const WString& path, const WString& name, DirectoryEntry* parent);

			ResourceMetaPtr meta;
			std::time_t lastUpdateTime;
		};

		struct DirectoryEntry : public LibraryEntry
		{
			DirectoryEntry();
			DirectoryEntry(const WString& path, const WString& name, DirectoryEntry* parent);

			Vector<LibraryEntry*>::type mChildren;
		};

	public:
		ProjectLibrary(const WString& projectFolder);
		~ProjectLibrary();

		void update();
		void checkForModifications(const WString& fullPath);

		const LibraryEntry* getRootEntry() const { return mRootEntry; }
		LibraryEntry* findEntry(const WString& fullPath) const;

		void moveEntry(const WString& oldPath, const WString& newPath);
		void deleteEntry(const WString& path);

		boost::signal<void(const WString&)> onEntryRemoved;
		boost::signal<void(const WString&)> onEntryAdded;
	private:
		static const WString RESOURCES_DIR;
		static const WString INTERNAL_RESOURCES_DIR;
		static const WString LIBRARY_ENTRIES_FILENAME;
		static const WString RESOURCE_MANIFEST_FILENAME;

		ResourceManifestPtr mResourceManifest;
		DirectoryEntry* mRootEntry;
		FolderMonitor* mMonitor;
		WString mProjectFolder;
		WString mResourcesFolder;

		void save();
		void load();

		ResourceEntry* addResourceInternal(DirectoryEntry* parent, const WString& filePath);
		DirectoryEntry* addDirectoryInternal(DirectoryEntry* parent, const WString& dirPath);

		void deleteResourceInternal(ResourceEntry* resource);
		void deleteDirectoryInternal(DirectoryEntry* directory);

		void reimportResourceInternal(ResourceEntry* resource);

		void createInternalParentHierarchy(const WString& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf);

		bool isUpToDate(ResourceEntry* resource) const;
		WString getMetaPath(const WString& path) const;
		bool isMeta(const WString& fullPath) const;

		void onMonitorFileModified(const WString& path);
	};
}