#pragma once

#include "BsEditorPrerequisites.h"
#include "BsProjectLibrary.h"
#include "CmIReflectable.h"

namespace BansheeEngine
{
	class ProjectLibraryEntries : public IReflectable
	{
		struct ConstructPrivately { };

	public:
		explicit ProjectLibraryEntries(const ConstructPrivately& dummy);
		ProjectLibraryEntries(const ProjectLibrary::DirectoryEntry& rootEntry);

		static std::shared_ptr<ProjectLibraryEntries> create(const ProjectLibrary::DirectoryEntry& rootEntry);
		
		const ProjectLibrary::DirectoryEntry& getRootEntry() const { return mRootEntry; }

	private:
		ProjectLibrary::DirectoryEntry mRootEntry;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
		static std::shared_ptr<ProjectLibraryEntries> createEmpty();

	public:
		friend class ProjectLibraryEntriesRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}