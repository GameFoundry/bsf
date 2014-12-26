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
#include "BsResourceListenerManager.h"

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

	HResource Resources::loadFromUUID(const String& uuid, bool async)
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

			HResource outputResource(uuid);
			loadComplete(outputResource);

			return outputResource;
		}

		return loadInternal(filePath, !async);
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

		HResource outputResource;
		bool alreadyLoading = false;
		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mLoadedResources.find(uuid);
			if(iterFind != mLoadedResources.end()) // Resource is already loaded
			{
				outputResource = iterFind->second;
				alreadyLoading = true;
			}
		}

		if (!alreadyLoading) // If not already detected as loaded
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(uuid);
			if(iterFind2 != mInProgressResources.end()) 
			{
				outputResource = iterFind2->second->resource;

				// Previously being loaded as async but now we want it synced, so we wait
				if (synchronous)
					outputResource.blockUntilLoaded();

				alreadyLoading = true;
			}
		}

		// Not loaded and not in progress, start loading of new resource
		// (or if already loaded or in progress, load any dependencies)
		if (!alreadyLoading)
			outputResource = HResource(uuid);

		if(!FileSystem::isFile(filePath))
		{
			gDebug().logWarning("Specified file: " + filePath.toString() + " doesn't exist.");

			loadComplete(outputResource);
			return outputResource;
		}

		// Load saved resource data
		FileDecoder fs(filePath);
		SPtr<SavedResourceData> savedResourceData = std::static_pointer_cast<SavedResourceData>(fs.decode());

		// If already loading keep the old load operation active, 
		// otherwise create a new one
		if (!alreadyLoading)
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);

			ResourceLoadData* loadData = bs_new<ResourceLoadData>(outputResource, 0);
			mInProgressResources[uuid] = loadData;
			loadData->resource = outputResource;
			loadData->remainingDependencies = 1;
			loadData->notifyImmediately = synchronous; // Make resource listener trigger before exit if loading synchronously

			for (auto& dependency : savedResourceData->getDependencies())
			{
				if (dependency != uuid)
				{
					mDependantLoads[dependency].push_back(loadData);
					loadData->remainingDependencies++;
				}
			}
		}

		// Load dependencies
		{
			for (auto& dependency : savedResourceData->getDependencies())
			{
				loadFromUUID(dependency, !synchronous);
			}
		}

		// Actually queue the load
		if (!alreadyLoading)
		{
			if (synchronous || !savedResourceData->allowAsyncLoading())
			{
				loadCallback(filePath, outputResource);
			}
			else
			{
				String fileName = filePath.getFilename();
				String taskName = "Resource load: " + fileName;

				TaskPtr task = Task::create(taskName, std::bind(&Resources::loadCallback, this, filePath, outputResource));
				TaskScheduler::instance().addTask(task);
			}
		}
		else
			loadComplete(outputResource);

		return outputResource;
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
		if (resource == nullptr)
			return;

		if (!resource.isLoaded()) // If it's still loading wait until that finishes
		{
			LOGWRN("Performance warning: Unloading a resource that is still in process of loading \
				   causes a stall until resource finishes loading.");
			resource.blockUntilLoaded();
		}

		Vector<ResourceDependency> dependencies = Utility::findResourceDependencies(*resource.get());

		// Call this before we actually destroy it
		onResourceDestroyed(resource);

		resource->destroy();

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			mLoadedResources.erase(resource.getUUID());
		}

		resource._setHandleData(nullptr, "");

		for (auto& dependency : dependencies)
		{
			HResource dependantResource = dependency.resource;

			// Last reference was kept by the unloaded resource, so unload the dependency too
			if ((UINT32)dependantResource.mData.use_count() == (dependency.numReferences + 1))
			{
				// TODO - Use count is not thread safe. Meaning it might increase after above check, in
				// which case we will be unloading a resource that is in use. I don't see a way around 
				// it at the moment.

				unload(dependantResource);
			}
		}
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

		// Note: When unloading multiple resources it's possible that unloading one will also unload
		// another resource in "resourcesToUnload". This is fine because "unload" deals with invalid
		// handles gracefully.
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

		Vector<ResourceDependency> dependencyList = Utility::findResourceDependencies(*resource.get());

		Vector<String> dependencyUUIDs(dependencyList.size());
		for (UINT32 i = 0; i < (UINT32)dependencyList.size(); i++)
			dependencyUUIDs[i] = dependencyList[i].resource.getUUID();

		SPtr<SavedResourceData> resourceData = bs_shared_ptr<SavedResourceData>(dependencyUUIDs, resource->allowAsyncLoading());

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

	HResource Resources::_getResourceHandle(const String& uuid)
	{
		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mLoadedResources.find(uuid);
			if (iterFind != mLoadedResources.end()) // Resource is already loaded
			{
				return iterFind->second;
			}
		}

		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(uuid);
			if (iterFind2 != mInProgressResources.end())
			{
				return iterFind2->second->resource;
			}
		}

		return HResource();
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

	void Resources::loadComplete(HResource& resource)
	{
		String uuid = resource.getUUID();

		ResourceLoadData* myLoadData = nullptr;
		Vector<ResourceLoadData*> dependantLoads;
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind = mInProgressResources.find(uuid);
			if (iterFind != mInProgressResources.end())
			{
				myLoadData = iterFind->second;
				mInProgressResources.erase(iterFind);
			}

			dependantLoads = mDependantLoads[uuid];
			mDependantLoads.erase(uuid);
		}

		if (myLoadData != nullptr)
		{
			{
				BS_LOCK_MUTEX(mLoadedResourceMutex);
				mLoadedResources[uuid] = resource;
			}

			resource._setHandleData(myLoadData->loadedData, uuid);
			onResourceLoaded(resource);

			if (myLoadData->notifyImmediately)
				ResourceListenerManager::instance().notifyListeners(uuid);

			bs_delete(myLoadData);
		}

		for (auto& dependantLoad : dependantLoads)
		{
			dependantLoad->remainingDependencies--;

			if (dependantLoad->remainingDependencies == 0)
				loadComplete(dependantLoad->resource);
		}
	}

	void Resources::loadCallback(const Path& filePath, HResource& resource)
	{
		ResourcePtr rawResource = loadFromDiskAndDeserialize(filePath);

		bool finishLoad = false;
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);

			// Check if all my dependencies are loaded
			ResourceLoadData* myLoadData = mInProgressResources[resource.getUUID()];
			myLoadData->loadedData = rawResource;
			myLoadData->remainingDependencies--;

			finishLoad = myLoadData->remainingDependencies == 0;
		}

		if (finishLoad)
			loadComplete(resource);
	}

	BS_CORE_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}
}