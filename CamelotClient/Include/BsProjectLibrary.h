#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

namespace BansheeEditor
{
	class ProjectLibrary : public CM::Module<ProjectLibrary>
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
			LibraryEntry() {}
			LibraryEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent, LibraryEntryType type);

			LibraryEntryType type;
			CM::WString path;
			CM::WString elementName;

			DirectoryEntry* parent;
		};

		struct ResourceEntry : public LibraryEntry
		{
			ResourceEntry() {}
			ResourceEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent);

			ResourceMetaPtr meta;
			std::time_t lastUpdateTime;
		};

		struct DirectoryEntry : public LibraryEntry
		{
			DirectoryEntry() {}
			DirectoryEntry(const CM::WString& path, const CM::WString& name, DirectoryEntry* parent);

			CM::Vector<LibraryEntry*>::type mChildren;
		};

	public:
		ProjectLibrary();
		~ProjectLibrary();

		void update();
		void checkForModifications(const CM::WString& fullPath);

		const LibraryEntry* getRootEntry() const { return mRootEntry; }
		LibraryEntry* findEntry(const CM::WString& fullPath) const;

		void moveEntry(const CM::WString& oldPath, const CM::WString& newPath);
		void deleteEntry(const CM::WString& path);

		boost::signal<void(const CM::WString&)> onEntryRemoved;
		boost::signal<void(const CM::WString&)> onEntryAdded;
	private:
		static const CM::WString INTERNAL_RESOURCES_DIR;
		static const CM::WString LIBRARY_ENTRIES_FILENAME;
		static const CM::WString RESOURCE_MANIFEST_FILENAME;

		CM::ResourceManifestPtr mResourceManifest;
		DirectoryEntry* mRootEntry;
		CM::FolderMonitor* mMonitor;

		void save();
		void load();

		ResourceEntry* addResourceInternal(DirectoryEntry* parent, const CM::WString& filePath);
		DirectoryEntry* addDirectoryInternal(DirectoryEntry* parent, const CM::WString& dirPath);

		void deleteResourceInternal(ResourceEntry* resource);
		void deleteDirectoryInternal(DirectoryEntry* directory);

		void reimportResourceInternal(ResourceEntry* resource);

		void createInternalParentHierarchy(const CM::WString& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf);

		bool isUpToDate(ResourceEntry* resource) const;
		CM::WString getMetaPath(const CM::WString& path) const;
		bool isMeta(const CM::WString& fullPath) const;

		void onMonitorFileModified(const CM::WString& path);
	};
}