#include "BsResources.h"
#include "BsResource.h"
#include "BsResourceManifest.h"
#include "BsException.h"
#include "BsFileSerializer.h"
#include "BsFileSystem.h"
#include "BsTaskScheduler.h"
#include "BsUUID.h"
#include "BsPath.h"
#include "BsDebug.h"
#include "BsUtility.h"
#include "BsSavedResourceData.h"

namespace BansheeEngine
{
	Resources::Resources()
	{
		mDefaultResourceManifest = ResourceManifest::create("Default");
		mResourceManifests.push_back(mDefaultResourceManifest);
	}

	Resources::~Resources()
	{
		// Unload and invalidate all resources
		UnorderedMap<String, HResource> loadedResourcesCopy = mLoadedResources;

		for (auto& loadedResourcePair : loadedResourcesCopy)
		{
			unload(loadedResourcePair.second);

			// Invalidate the handle
			loadedResourcePair.second._setHandleData(nullptr, "");
		}
	}

	HResource Resources::load(const Path& filePath)
	{
		return loadInternal(filePath, true);
	}

	HResource Resources::loadAsync(const Path& filePath)
	{
		return loadInternal(filePath, false);
	}

	HResource Resources::loadFromUUID(const String& uuid)
	{
		Path filePath;
		bool foundPath = false;

		// Default manifest is at 0th index but all other take priority since Default manifest could
		// contain obsolete data. 
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->uuidToFilePath(uuid, filePath))
			{
				foundPath = true;
				break;
			}
		}

		if(!foundPath)
		{
			gDebug().logWarning("Cannot load resource. Resource with UUID '" + uuid + "' doesn't exist.");
			return HResource();
		}

		return load(filePath);
	}

	HResource Resources::loadFromUUIDAsync(const String& uuid)
	{
		Path filePath;
		bool foundPath = false;

		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->uuidToFilePath(uuid, filePath))
			{
				foundPath = true;
				break;
			}
		}

		if(!foundPath)
		{
			gDebug().logWarning("Cannot load resource. Resource with UUID '" + uuid + "' doesn't exist.");
			return HResource();
		}

		return loadAsync(filePath);
	}

	HResource Resources::loadInternal(const Path& filePath, bool synchronous)
	{
		String uuid;
		bool foundUUID = false;
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->filePathToUUID(filePath, uuid))
			{
				foundUUID = true;
				break;
			}
		}

		if(!foundUUID)
			uuid = UUIDGenerator::instance().generateRandom();

		// Load saved resource data
		{

		}

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mLoadedResources.find(uuid);
			if(iterFind != mLoadedResources.end()) // Resource is already loaded
			{
				return iterFind->second;
			}
		}

		bool resourceLoadingInProgress = false;
		HResource existingResource;

		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(uuid);
			if(iterFind2 != mInProgressResources.end()) 
			{
				existingResource = iterFind2->second;
				resourceLoadingInProgress = true;
			}
		}

		if(resourceLoadingInProgress) // We're already loading this resource
		{
			if(!synchronous)
				return existingResource;
			else
			{
				// Previously being loaded as async but now we want it synced, so we wait
				existingResource.blockUntilLoaded();

				return existingResource;
			}
		}

		if(!FileSystem::isFile(filePath))
		{
			gDebug().logWarning("Specified file: " + filePath.toString() + " doesn't exist.");
			return HResource();
		}

		HResource newResource(uuid);

		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			mInProgressResources[uuid] = newResource;
		}

		if(synchronous)
		{
			loadCallback(filePath, newResource);
		}
		else
		{
			String fileName = filePath.getFilename();
			String taskName = "Resource load: " + fileName;

			TaskPtr task = Task::create(taskName, std::bind(&Resources::loadCallback, this, filePath, newResource));
			TaskScheduler::instance().addTask(task);
		}

		return newResource;
	}

	ResourcePtr Resources::loadFromDiskAndDeserialize(const Path& filePath)
	{
		FileDecoder fs(filePath);
		fs.skip(); // Skipped over saved resource data
		std::shared_ptr<IReflectable> loadedData = fs.decode();

		if(loadedData == nullptr)
			BS_EXCEPT(InternalErrorException, "Unable to load resource.");

		if(!loadedData->isDerivedFrom(Resource::getRTTIStatic()))
			BS_EXCEPT(InternalErrorException, "Loaded class doesn't derive from Resource.");

		ResourcePtr resource = std::static_pointer_cast<Resource>(loadedData);
		return resource;
	}

	void Resources::unload(HResource resource)
	{
		if(!resource.isLoaded()) // If it's still loading wait until that finishes
			resource.blockUntilLoaded();

		// Call this before we actually destroy it
		onResourceDestroyed(resource);

		resource->destroy();

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			mLoadedResources.erase(resource.getUUID());
		}

		resource._setHandleData(nullptr, "");
	}

	void Resources::unloadAllUnused()
	{
		Vector<HResource> resourcesToUnload;

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			for(auto iter = mLoadedResources.begin(); iter != mLoadedResources.end(); ++iter)
			{
				if (iter->second.mData.unique()) // We just have this one reference, meaning nothing is using this resource
					resourcesToUnload.push_back(iter->second);
			}
		}

		for(auto iter = resourcesToUnload.begin(); iter != resourcesToUnload.end(); ++iter)
		{
			unload(*iter);
		}
	}

	void Resources::save(HResource resource, const Path& filePath, bool overwrite)
	{
		if(!resource.isLoaded())
			resource.blockUntilLoaded();

		bool fileExists = FileSystem::isFile(filePath);
		if(fileExists)
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				BS_EXCEPT(InvalidParametersException, "Another file exists at the specified location.");
		}

		mDefaultResourceManifest->registerResource(resource.getUUID(), filePath);

		Vector<HResource> dependencyList = Utility::findResourceDependencies(*resource.get(), false);
		SPtr<SavedResourceData> resourceData = bs_shared_ptr<SavedResourceData>(dependencyList, resource->allowAsyncLoading());

		FileEncoder fs(filePath);
		fs.encode(resourceData.get());
		fs.encode(resource.get());
	}

	void Resources::registerResourceManifest(const ResourceManifestPtr& manifest)
	{
		if(manifest->getName() == "Default")
			return;

		auto findIter = std::find(mResourceManifests.begin(), mResourceManifests.end(), manifest);
		if(findIter == mResourceManifests.end())
			mResourceManifests.push_back(manifest);
		else
			*findIter = manifest;
	}

	ResourceManifestPtr Resources::getResourceManifest(const String& name) const
	{
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if(name == (*iter)->getName())
				return (*iter);
		}

		return nullptr;
	}

	HResource Resources::_createResourceHandle(const ResourcePtr& obj)
	{
		String uuid = UUIDGenerator::instance().generateRandom();
		HResource newHandle(obj, uuid);

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);

			mLoadedResources[uuid] = newHandle;
		}
	
		return newHandle;
	}

	bool Resources::getFilePathFromUUID(const String& uuid, Path& filePath) const
	{
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->uuidToFilePath(uuid, filePath))
				return true;
		}

		return false;
	}

	bool Resources::getUUIDFromFilePath(const Path& path, String& uuid) const
	{
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->filePathToUUID(path, uuid))
				return true;
		}

		return false;
	}

	void Resources::loadCallback(const Path& filePath, HResource& resource)
	{
		ResourcePtr rawResource = loadFromDiskAndDeserialize(filePath);

		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			mInProgressResources.erase(resource.getUUID());
		}

		resource._setHandleData(rawResource, resource.getUUID());

		onResourceLoaded(resource);

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			mLoadedResources[resource.getUUID()] = resource;
		}
	}

	BS_CORE_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}
}