#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmUUID.h"

namespace CamelotEngine
{
	class Resources : public Module<Resources>
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	assetDatabasePath	Pathname of the asset database file. Path should not include file extension.
		 * 								If the database file doesn't exist it will be created in that location.
		 * 								Meta data for resources is stored in the asset database.
		 */
		Resources(const String& assetDatabasePath); // TODO
		~Resources(); // TODO

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
		ResourcePtr load(const String& filePath);

		/**
		 * @brief	Loads the resource with the given uuid.
		 *
		 * @param	uuid	UUID of the resource to load. 
		 *
		 * @return	Loaded resource, or null if it cannot be found.
		 */
		ResourcePtr load(const UUID& uuid);

		/**
		 * @brief	Saves the resource at the specified location.
		 *
		 * @param	resource	The resource.
		 * @param	filePath	Full pathname of the file.
		 */
		void save(ResourcePtr resource, const String& filePath);
	};
}