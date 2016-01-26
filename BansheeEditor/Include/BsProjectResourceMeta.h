//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains meta-data for a resource stored in the ProjectLibrary.
	 */
	class ProjectResourceMeta : public IReflectable
	{
	private:
		struct ConstructPrivately {};

	public:
		explicit ProjectResourceMeta(const ConstructPrivately&);

		/**
		 * @brief	Creates a new project library resource meta-data entry.
		 *
		 * @param	uuid				UUID of the resource.
		 * @param	typeId				RTTI type id of the resource.
		 * @param	resourceMetaData	Non-project library specific meta-data.
		 * @param	importOptions		Import options used for importing the resource.
		 *
		 * @return	New project library resource meta data instance.
		 */
		static ProjectResourceMetaPtr create(const String& uuid, UINT32 typeId, const ResourceMetaDataPtr& resourceMetaData,
			const ImportOptionsPtr& importOptions);

		/**
		 * @brief	Returns the UUID of the resource this meta data belongs to.
		 */
		const String& getUUID() const { return mUUID; }

		/**
		 * @brief	Returns the non-project library specific meta-data,
		 */
		ResourceMetaDataPtr getResourceMetaData() const { return mResourceMeta; }

		/**
		 * @brief	Returns the import options used for importing the resource this
		 *			object is referencing.
		 */
		const ImportOptionsPtr& getImportOptions() const { return mImportOptions; }

		/**
		 * @brief	Returns the RTTI type ID of the resource this object is referencing.
		 */
		UINT32 getTypeID() const { return mTypeId; }

		/**
		 * @brief	Checks should this resource always be included in the build, regardless if
		 *			it's being referenced or not.
		 */
		bool getIncludeInBuild() const { return mIncludeInBuild; }

		/**
		 * @brief	Determines if this resource will always be included in the build, regardless if
		 *			it's being referenced or not.
		 */
		void setIncludeInBuild(bool include) { mIncludeInBuild = include; }

	private:
		friend class ProjectLibrary;

		String mUUID;
		ResourceMetaDataPtr mResourceMeta;
		ImportOptionsPtr mImportOptions;
		UINT32 mTypeId;
		bool mIncludeInBuild;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty meta-data instance. Used only for serialization purposes.
		 */
		static ProjectResourceMetaPtr createEmpty();

	public:
		friend class ProjectResourceMetaRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;	
	};
}