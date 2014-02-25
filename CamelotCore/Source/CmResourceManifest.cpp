#include "CmResourceManifest.h"
#include "CmResourceManifestRTTI.h"
#include "CmPath.h"

namespace CamelotFramework
{
	void ResourceManifest::registerResource(const String& uuid, const WString& filePath)
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		WString standardizedFilePath = Path::standardizePath(filePath);
		StringUtil::toLowerCase(standardizedFilePath);

		if(iterFind != mUUIDToFilePath.end())
		{
			if(iterFind->second != standardizedFilePath)
			{
				mFilePathToUUID.erase(iterFind->second);

				mUUIDToFilePath[uuid] = standardizedFilePath;
				mFilePathToUUID[standardizedFilePath] = uuid;
			}
		}
		else
		{
			mUUIDToFilePath[uuid] = standardizedFilePath;
			mFilePathToUUID[standardizedFilePath] = uuid;
		}
	}

	void ResourceManifest::unregisterResource(const String& uuid)
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		if(iterFind != mUUIDToFilePath.end())
		{
			mFilePathToUUID.erase(iterFind->second);
			mUUIDToFilePath.erase(uuid);
		}
	}

	const WString& ResourceManifest::uuidToFilePath(const String& uuid) const
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		if(iterFind != mUUIDToFilePath.end())
			return iterFind->second;
		else
			return StringUtil::WBLANK;
	}

	const String& ResourceManifest::filePathToUUID(const WString& filePath) const
	{
		WString standardizedFilePath = Path::standardizePath(filePath);
		StringUtil::toLowerCase(standardizedFilePath);

		auto iterFind = mFilePathToUUID.find(standardizedFilePath);

		if(iterFind != mFilePathToUUID.end())
			return iterFind->second;
		else
			return StringUtil::BLANK;
	}

	bool ResourceManifest::uuidExists(const String& uuid) const
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		return iterFind != mUUIDToFilePath.end();
	}

	bool ResourceManifest::filePathExists(const WString& filePath) const
	{
		WString standardizedFilePath = Path::standardizePath(filePath);
		StringUtil::toLowerCase(standardizedFilePath);

		auto iterFind = mFilePathToUUID.find(standardizedFilePath);

		return iterFind != mFilePathToUUID.end();
	}

	RTTITypeBase* ResourceManifest::getRTTIStatic()
	{
		return ResourceManifestRTTI::instance();
	}

	RTTITypeBase* ResourceManifest::getRTTI() const
	{
		return ResourceManifest::getRTTIStatic();
	}
}