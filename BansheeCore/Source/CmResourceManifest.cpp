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
		return bs_shared_ptr<ResourceManifest>(name);
	}

	ResourceManifestPtr ResourceManifest::createEmpty()
	{
		return bs_shared_ptr<ResourceManifest>(ConstructPrivately());
	}

	void ResourceManifest::registerResource(const String& uuid, const Path& filePath)
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		if(iterFind != mUUIDToFilePath.end())
		{
			if (iterFind->second != filePath)
			{
				mFilePathToUUID.erase(iterFind->second);

				mUUIDToFilePath[uuid] = filePath;
				mFilePathToUUID[filePath] = uuid;
			}
		}
		else
		{
			mUUIDToFilePath[uuid] = filePath;
			mFilePathToUUID[filePath] = uuid;
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

	bool ResourceManifest::uuidToFilePath(const String& uuid, Path& filePath) const
	{
		auto iterFind = mUUIDToFilePath.find(uuid);

		if(iterFind != mUUIDToFilePath.end())
		{
			filePath = iterFind->second;
			return true;
		}
		else
		{
			filePath = Path::BLANK;
			return false;
		}
	}

	bool ResourceManifest::filePathToUUID(const Path& filePath, String& outUUID) const
	{
		auto iterFind = mFilePathToUUID.find(filePath);

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

	bool ResourceManifest::filePathExists(const Path& filePath) const
	{
		auto iterFind = mFilePathToUUID.find(filePath);

		return iterFind != mFilePathToUUID.end();
	}

	void ResourceManifest::save(const ResourceManifestPtr& manifest, const Path& path, const Path& relativePath)
	{
		ResourceManifestPtr copy = create(manifest->mName);

		for(auto& elem : manifest->mFilePathToUUID)
		{
			if (!relativePath.includes(elem.first))
			{
				BS_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" + 
					relativePath.toString() + "\". Path: \"" + elem.first.toString() + "\"");
			}

			Path elementRelativePath = elem.first.getRelative(relativePath);

			copy->mFilePathToUUID[elementRelativePath] = elem.second;
		}

		for(auto& elem : manifest->mUUIDToFilePath)
		{
			if(!relativePath.includes(elem.second))
			{
				BS_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" + 
					relativePath.toString() + "\". Path: \"" + elem.second.toString() + "\"");
			}

			Path elementRelativePath = elem.second.getRelative(relativePath);

			copy->mUUIDToFilePath[elem.first] = elementRelativePath;
		}

		FileSerializer fs;
		fs.encode(copy.get(), path);
	}

	ResourceManifestPtr ResourceManifest::load(const Path& path, const Path& relativePath)
	{
		FileSerializer fs;
		ResourceManifestPtr manifest = std::static_pointer_cast<ResourceManifest>(fs.decode(path));

		ResourceManifestPtr copy = create(manifest->mName);

		for(auto& elem : manifest->mFilePathToUUID)
		{
			Path absPath = elem.first.getAbsolute(relativePath);
			copy->mFilePathToUUID[absPath] = elem.second;
		}

		for(auto& elem : manifest->mUUIDToFilePath)
		{
			Path absPath = elem.second.getAbsolute(relativePath);
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