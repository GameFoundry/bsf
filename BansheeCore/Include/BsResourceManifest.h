#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Serializable class that contains UUID <-> file path mapping for resources.
	 * 			
	 * @note	This class allows you to reference resources between sessions. At the end of a session
	 * 			save the resource manifest, and then restore it at the start of a new session. This way
	 * 			ensures that resource UUIDs stay consistent and anything referencing them can find the
	 * 			resources.
	 *
	 *			Thread safe.
	 */
	class BS_CORE_EXPORT ResourceManifest : public IReflectable
	{
		struct ConstructPrivately {};
	public:
		explicit ResourceManifest(const ConstructPrivately& dummy);
		ResourceManifest(const String& name);

		/**
		 * @brief	Returns an unique name of the resource manifest.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Registers a new resource in the manifest.
		 */
		void registerResource(const String& uuid, const Path& filePath);

		/**
		 * @brief	Removes a resource from the manifest.
		 */
		void unregisterResource(const String& uuid);

		/**
		 * @brief	Attempts to find a resource with the provided UUID and outputs the path
		 *			to the resource if found. Returns true if UUID was found, false otherwise.
		 */
		bool uuidToFilePath(const String& uuid, Path& filePath) const;

		/**
		 * @brief	Attempts to find a resource with the provided path and outputs the UUID
		 *			to the resource if found. Returns true if path was found, false otherwise.
		 */
		bool filePathToUUID(const Path& filePath, String& outUUID) const;

		/**
		 * @brief	Checks if provided UUID exists in the manifest.
		 */
		bool uuidExists(const String& uuid) const;

		/**
		 * @brief	Checks if the provided path exists in the manifest.
		 */
		bool filePathExists(const Path& filePath) const;

		/**
		 * @brief	Saves the resource manifest to the specified location.
		 *
		 * @param	manifest		Manifest to save.
		 * @param	path			Full pathname of the file to save the manifest in.
		 * @param	relativePath	If not empty, all pathnames in the manifest will be stored
		 * 							as if relative to this path.
		 */
		static void save(const ResourceManifestPtr& manifest, const Path& path, const Path& relativePath);

		/**
		 * @brief	Loads the resource manifest from the specified location.
		 *
		 * @param	path			Full pathname of the file to load the manifest from.
		 * @param	relativePath	If not empty, all loaded pathnames will have this
		 * 							path prepended.
		 */
		static ResourceManifestPtr load(const Path& path, const Path& relativePath);

		/**
		 * @brief	Creates a new empty resource manifest. Provided name should be unique
		 *			among manifests.
		 */
		static ResourceManifestPtr create(const String& name);

	private:
		String mName;
		UnorderedMap<String, Path> mUUIDToFilePath;
		UnorderedMap<Path, String> mFilePathToUUID;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

		/**
		 * @brief	Creates a new empty resource manifest.
		 */
		static ResourceManifestPtr createEmpty();

	public:
		friend class ResourceManifestRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}