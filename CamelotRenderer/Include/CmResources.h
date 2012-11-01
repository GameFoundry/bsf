#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class Resources : public Module<Resources>
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	resMetaPath		Folder where the resource meta-data will be stored. If the folder doesn't exist
		 * 							it will be created.
		 */
		Resources(const String& metaDataFolder);
		~Resources();

		/**
		 * @brief	Loads the resource from a given path. Returns null if resource can't be loaded.
		 *
		 * @param	filePath					The path of the file to load. The file is searched for in
		 * 										the AssetDatabase first, and if it cannot be found it is
		 * 										loaded as a temporary resource object. You can't save
		 * 										references to temporary resource objects because they won't
		 * 										persist after application shut-down, but otherwise they act
		 * 										the same as normal resources.
		 *
		 * @return	Loaded resource, or null if it cannot be found.
		 */
		BaseResourceRef loadFromPath(const String& filePath);

		/**
		 * @brief	Loads the resource with the given uuid.
		 *
		 * @param	uuid	UUID of the resource to load. 
		 *
		 * @return	Loaded resource, or null if it cannot be found.
		 */
		BaseResourceRef loadFromUUID(const String& uuid);

		/**
		 * @brief	Saves the resource. Resource must be registered using Resources::create beforehand.
		 *
		 * @param	resource   	The resource.
		 */
		void save(BaseResourceRef resource);

		/**
		 * @brief	Creates a new resource at the specified location. Throws an exception if resource already exists.
		 * 			Automatically calls Resources::save.
		 *
		 * @param	resource 	The resource.
		 * @param	filePath 	Full pathname of the file.
		 * @param	overwrite	(optional) If true, any existing resource at the specified location
		 * 						will be overwritten.
		 */
		void create(BaseResourceRef resource, const String& filePath, bool overwrite = false);

	public:
		struct ResourceMetaData : public IReflectable
		{
			String mUUID;
			String mPath;

			/************************************************************************/
			/* 								SERIALIZATION                      		*/
			/************************************************************************/
		public:
			friend class ResourceMetaDataRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

	private:
		typedef std::shared_ptr<ResourceMetaData> ResourceMetaDataPtr;
		map<String, ResourceMetaDataPtr>::type mResourceMetaData;

		void loadMetaData();
		void saveMetaData(const ResourceMetaDataPtr metaData);

		void createMetaData(const String& uuid, const String& filePath);
		void addMetaData(const String& uuid, const String& filePath);
		void updateMetaData(const String& uuid, const String& newFilePath);
		void removeMetaData(const String& uuid);

		bool metaExists_UUID(const String& uuid) const;
		bool metaExists_Path(const String& path) const;

		const String& getPathFromUUID(const String& uuid) const;
		const String& getUUIDFromPath(const String& path) const;

		String mMetaDataFolderPath;
	};

	CM_EXPORT Resources& gResources();
}