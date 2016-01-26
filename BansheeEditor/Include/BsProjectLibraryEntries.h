//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsProjectLibrary.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a list of entries used by the ProjectLibrary. Used primarily
	 *			for serialization purposes, i.e. persisting ProjectLibrary state
	 *			between application runs.
	 */
	class ProjectLibraryEntries : public IReflectable
	{
		struct ConstructPrivately { };

	public:
		explicit ProjectLibraryEntries(const ConstructPrivately& dummy);
		ProjectLibraryEntries(const ProjectLibrary::DirectoryEntry& rootEntry);

		/**
		 * @brief	Creates new project library entries based on a ProjectLibrary root directory entry.
		 *
		 * @param	rootEntry	Root directory entry in ProjectLibrary.
		 */
		static std::shared_ptr<ProjectLibraryEntries> create(const ProjectLibrary::DirectoryEntry& rootEntry);
		
		/**
		 * @brief	Returns the root directory entry that references the entire entry hierarchy.
		 */
		const ProjectLibrary::DirectoryEntry& getRootEntry() const { return mRootEntry; }

	private:
		ProjectLibrary::DirectoryEntry mRootEntry;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/

		/**
		 * @brief	Creates new empty project library entries object. Used for serialization
		 *			purposes.
		 */
		static std::shared_ptr<ProjectLibraryEntries> createEmpty();

	public:
		friend class ProjectLibraryEntriesRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}