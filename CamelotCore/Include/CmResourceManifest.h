#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmPath.h"

namespace CamelotFramework
{
	/**
	 * @brief	Serializable class that contains UUID <-> file path mapping for resources.
	 * 			
	 * @note	This class allows you to reference resources between sessions. At the end of a session
	 * 			save the resource manifest, and then restore it at the start of your session. This way
	 * 			ensures that resource UUIDs stay consistent and anything referencing them can find the
	 * 			resources.
	 */
	class CM_EXPORT ResourceManifest : public IReflectable
	{
	public:
		void registerResource(const String& uuid, const WPath& filePath);
		void unregisterResource(const String& uuid);

		const WPath& uuidToFilePath(const String& uuid) const;
		const String& filePathToUUID(const WPath& filePath) const;

		bool uuidExists(const String& uuid) const;
		bool filePathExists(const WPath& filePath) const;

	private:
		Map<String, WPath>::type mUUIDToFilePath;
		Map<WPath, String>::type mFilePathToUUID;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceManifestRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}