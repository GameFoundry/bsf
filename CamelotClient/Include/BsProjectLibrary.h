#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include "CmPath.h"

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
			LibraryEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent, LibraryEntryType type);

			LibraryEntryType type;
			CM::WPath path;
			CM::WString elementName;

			DirectoryEntry* parent;
		};

		struct ResourceEntry : public LibraryEntry
		{
			ResourceEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent);

			ResourceMetaPtr meta;
			std::time_t lastUpdateTime;
		};

		struct DirectoryEntry : public LibraryEntry
		{
			DirectoryEntry(const CM::WPath& path, const CM::WString& name, DirectoryEntry* parent);

			CM::Vector<LibraryEntry*>::type mChildren;
		};

	public:
		ProjectLibrary();
		~ProjectLibrary();

		void update();
		void checkForModifications(const CM::WString& fullPath);

		const LibraryEntry* getRootEntry() const { return mRootEntry; }
		LibraryEntry* findEntry(const CM::WPath& fullPath) const;

		void moveEntry(const CM::WPath& oldPath, const CM::WPath& newPath);
		void deleteEntry(const CM::WPath& path);

		boost::signal<void(const CM::WPath&)> onEntryRemoved;
		boost::signal<void(const CM::WPath&)> onEntryAdded;
	private:
		static const CM::WString INTERNAL_RESOURCES_DIR;

		DirectoryEntry* mRootEntry;
		CM::FolderMonitor* mMonitor;

		ResourceEntry* addResourceInternal(DirectoryEntry* parent, const CM::WPath& filePath);
		DirectoryEntry* addDirectoryInternal(DirectoryEntry* parent, const CM::WPath& dirPath);

		void deleteResourceInternal(ResourceEntry* resource);
		void deleteDirectoryInternal(DirectoryEntry* directory);

		void reimportResourceInternal(ResourceEntry* resource);

		void createInternalParentHierarchy(const CM::WPath& fullPath, DirectoryEntry** newHierarchyRoot, DirectoryEntry** newHierarchyLeaf);

		bool isUpToDate(ResourceEntry* resource) const;
		CM::WPath getMetaPath(const CM::WPath& path) const;
		bool isMeta(const CM::WPath& fullPath) const;

		void onMonitorFileModified(const CM::WString& path);
	};
}