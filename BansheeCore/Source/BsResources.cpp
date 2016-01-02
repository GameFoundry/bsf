#include "BsResources.h"
#include "BsResource.h"
#include "BsResourceManifest.h"
#include "BsException.h"
#include "BsFileSerializer.h"
#include "BsFileSystem.h"
#include "BsTaskScheduler.h"
#include "BsUUID.h"
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
		UnorderedMap<String, LoadedResourceData> loadedResourcesCopy = mLoadedResources;

		for (auto& loadedResourcePair : loadedResourcesCopy)
			destroy(loadedResourcePair.second.resource);
	}

	HResource Resources::load(const Path& filePath, bool loadDependencies, bool keepInternalReference)
	{
		String uuid;
		bool foundUUID = getUUIDFromFilePath(filePath, uuid);

		if (!foundUUID)
			uuid = UUIDGenerator::generateRandom();

		return loadInternal(uuid, filePath, true, loadDependencies, keepInternalReference);
	}

	HResource Resources::load(const WeakResourceHandle<Resource>& handle, bool loadDependencies, bool keepInternalReference)
	{
		if (handle.mData == nullptr)
			return HResource();

		String uuid = handle.getUUID();
		return loadFromUUID(uuid, false, loadDependencies, keepInternalReference);
	}

	HResource Resources::loadAsync(const Path& filePath, bool loadDependencies, bool keepInternalReference)
	{
		String uuid;
		bool foundUUID = getUUIDFromFilePath(filePath, uuid);

		if (!foundUUID)
			uuid = UUIDGenerator::generateRandom();

		return loadInternal(uuid, filePath, false, loadDependencies, keepInternalReference);
	}

	HResource Resources::loadFromUUID(const String& uuid, bool async, bool loadDependencies, bool keepInternalReference)
	{
		Path filePath;

		// Default manifest is at 0th index but all other take priority since Default manifest could
		// contain obsolete data. 
		for (auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter)
		{
			if ((*iter)->uuidToFilePath(uuid, filePath))
				break;
		}

		return loadInternal(uuid, filePath, !async, loadDependencies, keepInternalReference);
	}

	HResource Resources::loadInternal(const String& UUID, const Path& filePath, bool synchronous, bool loadDependencies, bool keepInternalReference)
	{
		HResource outputResource;

		bool alreadyLoading = false;
		bool loadInProgress = false;
		{
			// Check if resource is already being loaded on a worker thread
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(UUID);
			if (iterFind2 != mInProgressResources.end())
			{
				LoadedResourceData& resData = iterFind2->second->resData;
				outputResource = resData.resource.lock();

				if (keepInternalReference)
				{
					resData.numInternalRefs++;
					outputResource.addInternalRef();
				}

				alreadyLoading = true;
				loadInProgress = true;
			}

			// Previously being loaded as async but now we want it synced, so we wait
			if (loadInProgress && synchronous)
				outputResource.blockUntilLoaded();

			if (!alreadyLoading)
			{
				BS_LOCK_MUTEX(mLoadedResourceMutex);
				auto iterFind = mLoadedResources.find(UUID);
				if (iterFind != mLoadedResources.end()) // Resource is already loaded
				{
					LoadedResourceData& resData = iterFind->second;
					outputResource = resData.resource.lock();

					if (keepInternalReference)
					{
						resData.numInternalRefs++;
						outputResource.addInternalRef();
					}

					alreadyLoading = true;
				}
			}
		}


		// Not loaded and not in progress, start loading of new resource
		// (or if already loaded or in progress, load any dependencies)
		if (!alreadyLoading)
		{
			// Check if the handle already exists
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mHandles.find(UUID);
			if (iterFind != mHandles.end())
				outputResource = iterFind->second.lock();
			else
			{
				outputResource = HResource(UUID);
				mHandles[UUID] = outputResource.getWeak();
			}			
		}

		// We have nowhere to load from, warn and complete load if a file path was provided,
		// otherwise pass through as we might just want to load from memory. 
		if (filePath.isEmpty())
		{
			if (!alreadyLoading)
			{
				LOGWRN_VERBOSE("Cannot load resource. Resource with UUID '" + UUID + "' doesn't exist.");

				// Complete the load as that the depedency counter is properly reduced, in case this 
				// is a dependency of some other resource.
				loadComplete(outputResource);
				return outputResource;
			}
		}
		else if (!FileSystem::isFile(filePath))
		{
			LOGWRN_VERBOSE("Cannot load resource. Specified file: " + filePath.toString() + " doesn't exist.");

			// Complete the load as that the depedency counter is properly reduced, in case this 
			// is a dependency of some other resource.
			loadComplete(outputResource);
			assert(!loadInProgress); // Resource already being loaded but we can't find its path now?

			return outputResource;
		}

		// Load dependency data if a file path is provided
		SPtr<SavedResourceData> savedResourceData;
		if (!filePath.isEmpty())
		{
			FileDecoder fs(filePath);
			savedResourceData = std::static_pointer_cast<SavedResourceData>(fs.decode());
		}

		// If already loading keep the old load operation active, otherwise create a new one
		if (!alreadyLoading)
		{
			{
				BS_LOCK_MUTEX(mInProgressResourcesMutex);

				ResourceLoadData* loadData = bs_new<ResourceLoadData>(outputResource.getWeak(), 0);
				mInProgressResources[UUID] = loadData;
				loadData->resData = outputResource.getWeak();

				if (keepInternalReference)
				{
					loadData->resData.numInternalRefs++;
					outputResource.addInternalRef();
				}

				loadData->remainingDependencies = 1;
				loadData->notifyImmediately = synchronous; // Make resource listener trigger before exit if loading synchronously

				// Register dependencies and count them so we know when the resource is fully loaded
				if (loadDependencies && savedResourceData != nullptr)
				{
					for (auto& dependency : savedResourceData->getDependencies())
					{
						if (dependency != UUID)
						{
							mDependantLoads[dependency].push_back(loadData);
							loadData->remainingDependencies++;
						}
					}
				}
			}

			if (loadDependencies && savedResourceData != nullptr)
			{
				const Vector<String>& dependencyUUIDs = savedResourceData->getDependencies();
				UINT32 numDependencies = (UINT32)dependencyUUIDs.size();
				Vector<HResource> dependencies(numDependencies);

				for (UINT32 i = 0; i < numDependencies; i++)
					dependencies[i] = loadFromUUID(dependencyUUIDs[i], !synchronous, true, false);

				// Keep dependencies alive until the parent is done loading
				{
					BS_LOCK_MUTEX(mInProgressResourcesMutex);

					// At this point the resource is guaranteed to still be in-progress, so it's safe to update its dependency list
					mInProgressResources[UUID]->dependencies = dependencies;
				}
			}
		}
		else if (loadDependencies && savedResourceData != nullptr) // Queue dependencies in case they aren't already loaded
		{
			const Vector<String>& dependencies = savedResourceData->getDependencies();
			if (!dependencies.empty())
			{
				{
					BS_LOCK_MUTEX(mInProgressResourcesMutex);

					ResourceLoadData* loadData = nullptr;

					auto iterFind = mInProgressResources.find(UUID);
					if (iterFind == mInProgressResources.end()) // Fully loaded
					{
						loadData = bs_new<ResourceLoadData>(outputResource.getWeak(), 0);
						loadData->resData = outputResource.getWeak();
						loadData->remainingDependencies = 0;
						loadData->notifyImmediately = synchronous; // Make resource listener trigger before exit if loading synchronously

						mInProgressResources[UUID] = loadData;
					}
					else
					{
						loadData = iterFind->second;
					}

					// Register dependencies and count them so we know when the resource is fully loaded
					for (auto& dependency : dependencies)
					{
						if (dependency != UUID)
						{
							bool registerDependency = true;

							auto iterFind2 = mDependantLoads.find(dependency);
							if (iterFind2 != mDependantLoads.end())
							{
								Vector<ResourceLoadData*>& dependantData = iterFind2->second;
								auto iterFind3 = std::find_if(dependantData.begin(), dependantData.end(),
									[&](ResourceLoadData* x)
								{
									return x->resData.resource.getUUID() == outputResource.getUUID();
								});

								registerDependency = iterFind3 == dependantData.end();
							}

							if (registerDependency)
							{
								mDependantLoads[dependency].push_back(loadData);
								loadData->remainingDependencies++;
								loadData->dependencies.push_back(_getResourceHandle(dependency));
							}
						}
					}
				}

				for (auto& dependency : dependencies)
					loadFromUUID(dependency, !synchronous, true, false);
			}
		}

		// Actually start the file read operation if not already loaded or in progress
		if (!alreadyLoading && !filePath.isEmpty())
		{
			// Synchronous or the resource doesn't support async, read the file immediately
			if (synchronous || !savedResourceData->allowAsyncLoading())
			{
				loadCallback(filePath, outputResource);
			}
			else // Asynchronous, read the file on a worker thread
			{
				String fileName = filePath.getFilename();
				String taskName = "Resource load: " + fileName;

				TaskPtr task = Task::create(taskName, std::bind(&Resources::loadCallback, this, filePath, outputResource));
				TaskScheduler::instance().addTask(task);
			}
		}
		else // File already loaded or in progress
		{
			// Complete the load unless its in progress in which case we wait for its worker thread to complete it.
			// In case file is already loaded this will only decrement dependency count in case this resource is a dependency.
			if (!loadInProgress)
				loadComplete(outputResource);
			else
			{
				// In case loading finished in the meantime we cannot be sure at what point ::loadComplete was triggered,
				// so trigger it manually so that the dependency count is properly decremented in case this resource
				// is a dependency.
				BS_LOCK_MUTEX(mLoadedResourceMutex);
				auto iterFind = mLoadedResources.find(UUID);
				if (iterFind != mLoadedResources.end())
					loadComplete(outputResource);
			}
		}

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

	void Resources::release(ResourceHandleBase& resource)
	{
		const String& UUID = resource.getUUID();

		{
			bool loadInProgress = false;

			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(UUID);
			if (iterFind2 != mInProgressResources.end())
				loadInProgress = true;

			// Technically we should be able to just cancel a load in progress instead of blocking until it finishes.
			// However that would mean the last reference could get lost on whatever thread did the loading, which
			// isn't something that's supported. If this ends up being a problem either make handle counting atomic
			// or add a separate queue for objects destroyed from the load threads.
			if (loadInProgress)
				resource.blockUntilLoaded();

			{
				BS_LOCK_MUTEX(mLoadedResourceMutex);
				auto iterFind = mLoadedResources.find(UUID);
				if (iterFind != mLoadedResources.end()) // Resource is already loaded
				{
					LoadedResourceData& resData = iterFind->second;

					assert(resData.numInternalRefs > 0);
					resData.numInternalRefs--;
					resource.removeInternalRef();

					return;
				}
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
				const LoadedResourceData& resData = iter->second;

				if (resData.resource.mData->mRefCount == resData.numInternalRefs) // Only internal references exist, free it
					resourcesToUnload.push_back(resData.resource.lock());
			}
		}

		// Note: When unloading multiple resources it's possible that unloading one will also unload
		// another resource in "resourcesToUnload". This is fine because "unload" deals with invalid
		// handles gracefully.
		for(auto iter = resourcesToUnload.begin(); iter != resourcesToUnload.end(); ++iter)
		{
			release(*iter);
		}
	}

	void Resources::destroy(ResourceHandleBase& resource)
	{
		if (resource.mData == nullptr)
			return;

		const String& uuid = resource.getUUID();
		if (!resource.isLoaded(false))
		{
			bool loadInProgress = false;
			{
				BS_LOCK_MUTEX(mInProgressResourcesMutex);
				auto iterFind2 = mInProgressResources.find(uuid);
				if (iterFind2 != mInProgressResources.end())
					loadInProgress = true;
			}

			if (loadInProgress) // If it's still loading wait until that finishes
				resource.blockUntilLoaded();
			else
				return; // Already unloaded
		}

		// Notify external systems before we actually destroy it
		onResourceDestroyed(uuid);
		resource.mData->mPtr->destroy();

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);

			auto iterFind = mLoadedResources.find(uuid);
			if (iterFind != mLoadedResources.end())
			{
				LoadedResourceData& resData = iterFind->second;
				while (resData.numInternalRefs > 0)
				{
					resData.numInternalRefs--;
					resData.resource.removeInternalRef();
				}

				mLoadedResources.erase(iterFind);
			}
			else
			{
				assert(false); // This should never happen but in case it does fail silently in release mode
			}
		}

		resource.setHandleData(nullptr, uuid);
	}

	void Resources::save(const HResource& resource, const Path& filePath, bool overwrite)
	{
		if (resource == nullptr)
			return;

		if (!resource.isLoaded(false))
		{
			bool loadInProgress = false;
			{
				BS_LOCK_MUTEX(mInProgressResourcesMutex);
				auto iterFind2 = mInProgressResources.find(resource.getUUID());
				if (iterFind2 != mInProgressResources.end())
					loadInProgress = true;
			}

			if (loadInProgress) // If it's still loading wait until that finishes
				resource.blockUntilLoaded();
			else
				return; // Nothing to save
		}

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

		SPtr<SavedResourceData> resourceData = bs_shared_ptr_new<SavedResourceData>(dependencyUUIDs, resource->allowAsyncLoading());

		FileEncoder fs(filePath);
		fs.encode(resourceData.get());
		fs.encode(resource.get());
	}

	void Resources::save(const HResource& resource)
	{
		if (resource == nullptr)
			return;

		Path path;
		if (getFilePathFromUUID(resource.getUUID(), path))
			save(resource, path, true);
	}

	void Resources::update(HResource& handle, const ResourcePtr& resource)
	{
		const String& uuid = handle.getUUID();
		handle.setHandleData(resource, uuid);

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mLoadedResources.find(uuid);
			if (iterFind == mLoadedResources.end())
			{
				LoadedResourceData& resData = mLoadedResources[uuid];
				resData.resource = handle.getWeak();
			}
		}

		onResourceModified(handle);
	}

	Vector<String> Resources::getDependencies(const Path& filePath)
	{
		SPtr<SavedResourceData> savedResourceData;
		if (!filePath.isEmpty())
		{
			FileDecoder fs(filePath);
			savedResourceData = std::static_pointer_cast<SavedResourceData>(fs.decode());
		}

		return savedResourceData->getDependencies();
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

	void Resources::unregisterResourceManifest(const ResourceManifestPtr& manifest)
	{
		if (manifest->getName() == "Default")
			return;

		auto findIter = std::find(mResourceManifests.begin(), mResourceManifests.end(), manifest);
		if (findIter != mResourceManifests.end())
			mResourceManifests.erase(findIter);
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

	bool Resources::isLoaded(const String& uuid, bool checkInProgress)
	{
		if (checkInProgress)
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(uuid);
			if (iterFind2 != mInProgressResources.end())
			{
				return true;
			}

			{
				BS_LOCK_MUTEX(mLoadedResourceMutex);
				auto iterFind = mLoadedResources.find(uuid);
				if (iterFind != mLoadedResources.end())
				{
					return true;
				}
			}
		}

		return false;
	}

	HResource Resources::_createResourceHandle(const ResourcePtr& obj)
	{
		String uuid = UUIDGenerator::generateRandom();
		HResource newHandle(obj, uuid);

		{
			BS_LOCK_MUTEX(mLoadedResourceMutex);

			LoadedResourceData& resData = mLoadedResources[uuid];
			resData.resource = newHandle.getWeak();
			mHandles[uuid] = newHandle.getWeak();
		}
	
		return newHandle;
	}

	HResource Resources::_getResourceHandle(const String& uuid)
	{
		BS_LOCK_MUTEX(mLoadedResourceMutex);
		auto iterFind3 = mHandles.find(uuid);
		if (iterFind3 != mHandles.end()) // Not loaded, but handle does exist
		{
			return iterFind3->second.lock();
		}

		// Create new handle
		HResource handle(uuid);
		mHandles[uuid] = handle.getWeak();

		return handle;
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
		Path manifestPath = path;
		if (!manifestPath.isAbsolute())
			manifestPath.makeAbsolute(FileSystem::getWorkingDirectoryPath());

		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if ((*iter)->filePathToUUID(manifestPath, uuid))
				return true;
		}

		return false;
	}

	void Resources::loadComplete(HResource& resource)
	{
		String uuid = resource.getUUID();

		ResourceLoadData* myLoadData = nullptr;
		bool finishLoad = true;
		Vector<ResourceLoadData*> dependantLoads;
		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);

			auto iterFind = mInProgressResources.find(uuid);
			if (iterFind != mInProgressResources.end())
			{
				myLoadData = iterFind->second;
				finishLoad = myLoadData->remainingDependencies == 0;
				
				if (finishLoad)
					mInProgressResources.erase(iterFind);
			}

			auto iterFind2 = mDependantLoads.find(uuid);

			if (iterFind2 != mDependantLoads.end())
				dependantLoads = iterFind2->second;

			if (finishLoad)
			{
				mDependantLoads.erase(uuid);

				// If loadedData is null then we're probably completing load on an already loaded resource, triggered
				// by its dependencies.
				if (myLoadData != nullptr && myLoadData->loadedData != nullptr)
				{
					BS_LOCK_MUTEX(mLoadedResourceMutex);

					mLoadedResources[uuid] = myLoadData->resData;
					resource.setHandleData(myLoadData->loadedData, uuid);
				}

				for (auto& dependantLoad : dependantLoads)
					dependantLoad->remainingDependencies--;
			}
		}

		for (auto& dependantLoad : dependantLoads)
		{
			HResource dependant = dependantLoad->resData.resource.lock();
			loadComplete(dependant);
		}

		if (finishLoad && myLoadData != nullptr)
		{
			onResourceLoaded(resource);

			// This should only ever be true on the main thread
			if (myLoadData->notifyImmediately)
				ResourceListenerManager::instance().notifyListeners(uuid);

			bs_delete(myLoadData);
		}
	}

	void Resources::loadCallback(const Path& filePath, HResource& resource)
	{
		ResourcePtr rawResource = loadFromDiskAndDeserialize(filePath);

		{
			BS_LOCK_MUTEX(mInProgressResourcesMutex);

			// Check if all my dependencies are loaded
			ResourceLoadData* myLoadData = mInProgressResources[resource.getUUID()];
			myLoadData->loadedData = rawResource;
			myLoadData->remainingDependencies--;
		}

		loadComplete(resource);
	}

	BS_CORE_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}
}