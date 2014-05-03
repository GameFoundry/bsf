#include "CmResourceManifest.h"
#include "CmResourceManifestRTTI.h"
#include "CmPath.h"
#include "CmFileSerializer.h"
#include "CmException.h"

namespace BansheeEngine
{
	ResourceManifest::ResourceManifest(const ConstructPrivately& dummy)
	{

	}

	ResourceManifest::ResourceManifest(const String& name)
		:mName(name)
	{

	}

	ResourceManifestPtr ResourceManifest::create(const String& name)
	{
		return cm_shared_ptr<ResourceManifest>(name);
	}

	ResourceManifestPtr ResourceManifest::createEmpty()
	{
		return cm_shared_ptr<ResourceManifest>(ConstructPrivately());
	}

	void ResourceManifest::registerResource(const String& uuid, const WString& filePath)
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		WString standardizedFilePath = OldPath::standardizePath(filePath);
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

	bool ResourceManifest::uuidToFilePath(const String& uuid, WString& filePath) const
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		if(iterFind != mUUIDToFilePath.end())
		{
			filePath = iterFind->second;
			return true;
		}
		else
		{
			filePath = StringUtil::WBLANK;
			return false;
		}
	}

	bool ResourceManifest::filePathToUUID(const WString& filePath, String& outUUID) const
	{
		WString standardizedFilePath = OldPath::standardizePath(filePath);
		StringUtil::toLowerCase(standardizedFilePath);

		auto iterFind = mFilePathToUUID.find(standardizedFilePath);

		if(iterFind != mFilePathToUUID.end())
		{
			outUUID = iterFind->second;
			return true;
		}
		else
		{
			outUUID = StringUtil::BLANK;
			return false;
		}
	}

	bool ResourceManifest::uuidExists(const String& uuid) const
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		return iterFind != mUUIDToFilePath.end();
	}

	bool ResourceManifest::filePathExists(const WString& filePath) const
	{
		WString standardizedFilePath = OldPath::standardizePath(filePath);
		StringUtil::toLowerCase(standardizedFilePath);

		auto iterFind = mFilePathToUUID.find(standardizedFilePath);

		return iterFind != mFilePathToUUID.end();
	}

	void ResourceManifest::save(const ResourceManifestPtr& manifest, const WString& path, const WString& relativePath)
	{
		WString standRelativePath = OldPath::standardizePath(relativePath);
		StringUtil::toLowerCase(standRelativePath);

		ResourceManifestPtr copy = create(manifest->mName);

		for(auto& elem : manifest->mFilePathToUUID)
		{
			if(!OldPath::includes(elem.first, standRelativePath))
			{
				CM_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" + 
					toString(relativePath) + "\". Path: \"" + toString(elem.first) + "\"");
			}

			WString relativePath = OldPath::relative(standRelativePath, elem.first);

			copy->mFilePathToUUID[relativePath] = elem.second;
		}

		for(auto& elem : manifest->mUUIDToFilePath)
		{
			if(!OldPath::includes(elem.second, standRelativePath))
			{
				CM_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" + 
					toString(relativePath) + "\". Path: \"" + toString(elem.second) + "\"");
			}

			WString relativePath = OldPath::relative(standRelativePath, elem.second);

			copy->mUUIDToFilePath[elem.first] = relativePath;
		}

		FileSerializer fs;
		fs.encode(copy.get(), path);
	}

	ResourceManifestPtr ResourceManifest::load(const WString& path, const WString& relativePath)
	{
		WString standRelativePath = OldPath::standardizePath(relativePath);
		StringUtil::toLowerCase(standRelativePath);

		FileSerializer fs;
		ResourceManifestPtr manifest = std::static_pointer_cast<ResourceManifest>(fs.decode(path));

		ResourceManifestPtr copy = create(manifest->mName);

		for(auto& elem : manifest->mFilePathToUUID)
		{
			WString absPath = OldPath::combine(standRelativePath, elem.first);
			copy->mFilePathToUUID[absPath] = elem.second;
		}

		for(auto& elem : manifest->mUUIDToFilePath)
		{
			WString absPath = OldPath::combine(standRelativePath, elem.second);
			copy->mUUIDToFilePath[elem.first] = absPath;
		}

		return copy;
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