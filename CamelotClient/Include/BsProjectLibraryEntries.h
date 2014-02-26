#pragma once

#include "BsEditorPrerequisites.h"
#include "BsProjectLibrary.h"
#include "CmIReflectable.h"

namespace BansheeEditor
{
	class ProjectLibraryEntries : public CM::IReflectable
	{
		struct ConstructPrivately { };

	public:
		explicit ProjectLibraryEntries(const ConstructPrivately& dummy);
		ProjectLibraryEntries(const ProjectLibrary::DirectoryEntry& rootEntry);

		static std::shared_ptr<ProjectLibraryEntries> create(const ProjectLibrary::DirectoryEntry& rootEntry);
		static std::shared_ptr<ProjectLibraryEntries> createEmpty();

		const ProjectLibrary::DirectoryEntry& getRootEntry() const { return mRootEntry; }

	private:
		ProjectLibrary::DirectoryEntry mRootEntry;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/

	public:
		friend class ProjectLibraryEntriesRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}