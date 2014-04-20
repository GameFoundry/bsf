#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Serializable class that contains UUID <-> file path mapping for resources.
	 * 			
	 * @note	This class allows you to reference resources between sessions. At the end of a session
	 * 			save the resource manifest, and then restore it at the start of a new session. This way
	 * 			ensures that resource UUIDs stay consistent and anything referencing them can find the
	 * 			resources.
	 */
	class CM_EXPORT ResourceManifest : public IReflectable
	{
		struct ConstructPrivately {};
	public:
		explicit ResourceManifest(const ConstructPrivately& dummy);
		ResourceManifest(const String& name);

		const String& getName() const { return mName; }

		void registerResource(const String& uuid, const WString& filePath);
		void unregisterResource(const String& uuid);

		bool uuidToFilePath(const String& uuid, WString& filePath) const;
		bool filePathToUUID(const WString& filePath, String& outUUID) const;

		bool uuidExists(const String& uuid) const;
		bool filePathExists(const WString& filePath) const;

		/**
		 * @brief	Saves the resource manifest to the specified location.
		 *
		 * @param	manifest		Manifest to save.
		 * @param	path			Full pathname of the file.
		 * @param	relativePath	If not empty, all pathnames in the manifest will be stored
		 * 							as if relative to this path.
		 */
		static void save(const ResourceManifestPtr& manifest, const WString& path, const WString& relativePath);

		/**
		 * @brief	Loads the resource manifest from the specified location.
		 *
		 * @param	path			Full pathname of the file.
		 * @param	relativePath	If not empty, all loaded pathnames will have this
		 * 							path prepended.
		 */
		static ResourceManifestPtr load(const WString& path, const WString& relativePath);

		static ResourceManifestPtr create(const String& name);

	private:
		String mName;
		Map<String, WString>::type mUUIDToFilePath;
		Map<WString, String>::type mFilePathToUUID;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		static ResourceManifestPtr createEmpty();

	public:
		friend class ResourceManifestRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}