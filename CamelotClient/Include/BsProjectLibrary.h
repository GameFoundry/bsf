#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include "CmPath.h"

namespace BansheeEditor
{
	class ProjectLibrary : public CM::Module<ProjectLibrary>
	{
		struct LibraryEntry;
		struct ResourceEntry;
		struct DirectoryEntry;

	public:
		ProjectLibrary();
		~ProjectLibrary();

		void checkForModifications(const CM::WString& folder);

	private:
		static const CM::WString INTERNAL_RESOURCES_DIR;

		DirectoryEntry* mRootEntry;

		void addResource(DirectoryEntry* parent, const CM::WPath& filePath);
		void reimportResource(ResourceEntry* resource);

		void deleteResource(ResourceEntry* resource);
		void deleteDirectory(DirectoryEntry* directory);

		bool isUpToDate(ResourceEntry* resource) const;

		DirectoryEntry* findDirectoryEntry(const CM::WString& folder) const;
	};
}