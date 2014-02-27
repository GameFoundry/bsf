#include "CmResources.h"
#include "CmResource.h"
#include "CmResourceManifest.h"
#include "CmException.h"
#include "CmFileSerializer.h"
#include "CmFileSystem.h"
#include "CmUUID.h"
#include "CmPath.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	bool Resources::ResourceRequestHandler::canHandleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
	{
		return true;
	}

	WorkQueue::Response* Resources::ResourceRequestHandler::handleRequest(WorkQueue::Request* req, const WorkQueue* srcQ)
	{
		ResourceLoadRequestPtr resRequest = boost::any_cast<ResourceLoadRequestPtr>(req->getData());

		ResourceLoadResponsePtr resResponse = cm_shared_ptr<Resources::ResourceLoadResponse, ScratchAlloc>();
		resResponse->rawResource = gResources().loadFromDiskAndDeserialize(resRequest->filePath);

		return cm_new<WorkQueue::Response, ScratchAlloc>(req, true, resResponse);
	}

	bool Resources::ResourceResponseHandler::canHandleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
	{
		return true;
	}

	void Resources::ResourceResponseHandler::handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
	{
		ResourceLoadRequestPtr resRequest = boost::any_cast<ResourceLoadRequestPtr>(res->getRequest()->getData());

		if(res->getRequest()->getAborted())
			return;

		gResources().notifyResourceLoadingFinished(resRequest->resource);

		if(res->succeeded())
		{
			ResourceLoadResponsePtr resResponse = boost::any_cast<ResourceLoadResponsePtr>(res->getData());
			
			// This should be thread safe without any sync primitives, if other threads read a few cycles out of date value
			// and think this resource isn't created when it really is, it hardly makes any difference
			resRequest->resource._setHandleData(resResponse->rawResource, resRequest->resource.getUUID());

			gResources().notifyNewResourceLoaded(resRequest->resource);
		}
		else
		{
			gDebug().logWarning("Resource load request failed.");
		}
	}

	Resources::Resources()
		:mRequestHandler(nullptr), mResponseHandler(nullptr), mWorkQueue(nullptr)
	{
		mDefaultResourceManifest = ResourceManifest::create("Default");
		mResourceManifests.push_back(mDefaultResourceManifest);

		mWorkQueue = cm_new<WorkQueue>();
		mWorkQueueChannel = mWorkQueue->getChannel("Resources");
		mRequestHandler = cm_new<ResourceRequestHandler>();
		mResponseHandler = cm_new<ResourceResponseHandler>();

		mWorkQueue->addRequestHandler(mWorkQueueChannel, mRequestHandler);
		mWorkQueue->addResponseHandler(mWorkQueueChannel, mResponseHandler);

		// TODO Low priority - I might want to make this more global so other classes can use it
#if CM_THREAD_SUPPORT
		mWorkQueue->setWorkerThreadCount(CM_THREAD_HARDWARE_CONCURRENCY);
#endif
		mWorkQueue->startup();
	}

	Resources::~Resources()
	{
		if(mWorkQueue)
		{
			if(mRequestHandler != nullptr)
				mWorkQueue->removeRequestHandler(mWorkQueueChannel, mRequestHandler);

			if(mResponseHandler != nullptr)
				mWorkQueue->removeResponseHandler(mWorkQueueChannel, mResponseHandler);

			mWorkQueue->shutdown();

			cm_delete(mWorkQueue);
		}

		if(mRequestHandler != nullptr)
			cm_delete(mRequestHandler);

		if(mResponseHandler != nullptr)
			cm_delete(mResponseHandler);
	}

	HResource Resources::load(const WString& filePath)
	{
		return loadInternal(filePath, true);
	}

	HResource Resources::loadAsync(const WString& filePath)
	{
		return loadInternal(filePath, false);
	}

	HResource Resources::loadFromUUID(const String& uuid)
	{
		WString filePath;
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
		WString filePath;
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

	HResource Resources::loadInternal(const WString& filePath, bool synchronous)
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
			uuid = UUIDGenerator::generateRandom();

		{
			CM_LOCK_MUTEX(mLoadedResourceMutex);
			auto iterFind = mLoadedResources.find(uuid);
			if(iterFind != mLoadedResources.end()) // Resource is already loaded
			{
				return iterFind->second;
			}
		}

		bool resourceLoadingInProgress = false;
		HResource existingResource;

		{
			CM_LOCK_MUTEX(mInProgressResourcesMutex);
			auto iterFind2 = mInProgressResources.find(uuid);
			if(iterFind2 != mInProgressResources.end()) 
			{
				existingResource = iterFind2->second.resource;
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
				existingResource.synchronize();

				return existingResource;
			}
		}

		if(!FileSystem::isFile(filePath))
		{
			gDebug().logWarning("Specified file: " + toString(filePath) + " doesn't exist.");
			return HResource();
		}

		HResource newResource(uuid);

		ResourceLoadRequestPtr resRequest = cm_shared_ptr<Resources::ResourceLoadRequest, ScratchAlloc>();
		resRequest->filePath = filePath;
		resRequest->resource = newResource;

		WorkQueue::RequestID requestId = mWorkQueue->peekNextFreeRequestId();
		ResourceAsyncOp newAsyncOp;
		newAsyncOp.resource = newResource;
		newAsyncOp.requestID = requestId;

		{
			CM_LOCK_MUTEX(mInProgressResourcesMutex);
			mInProgressResources[uuid] = newAsyncOp;
		}

		mWorkQueue->addRequest(mWorkQueueChannel, resRequest, 0, synchronous);
		return newResource;
	}

	ResourcePtr Resources::loadFromDiskAndDeserialize(const WString& filePath)
	{
		FileSerializer fs;
		std::shared_ptr<IReflectable> loadedData = fs.decode(filePath);

		if(loadedData == nullptr)
			CM_EXCEPT(InternalErrorException, "Unable to load resource.");

		if(!loadedData->isDerivedFrom(Resource::getRTTIStatic()))
			CM_EXCEPT(InternalErrorException, "Loaded class doesn't derive from Resource.");

		ResourcePtr resource = std::static_pointer_cast<Resource>(loadedData);
		return resource;
	}

	void Resources::unload(HResource resource)
	{
		if(!resource.isLoaded()) // If it's still loading wait until that finishes
			resource.synchronize();

		resource->destroy();

		{
			CM_LOCK_MUTEX(mLoadedResourceMutex);
			mLoadedResources.erase(resource.getUUID());
		}
	}

	void Resources::unloadAllUnused()
	{
		Vector<HResource>::type resourcesToUnload;

		{
			CM_LOCK_MUTEX(mLoadedResourceMutex);
			for(auto iter = mLoadedResources.begin(); iter != mLoadedResources.end(); ++iter)
			{
				if(iter->second.getInternalPtr().unique()) // We just have this one reference, meaning nothing is using this resource
					resourcesToUnload.push_back(iter->second);
			}
		}

		for(auto iter = resourcesToUnload.begin(); iter != resourcesToUnload.end(); ++iter)
		{
			unload(*iter);
		}
	}

	void Resources::save(HResource resource, const WString& filePath, bool overwrite)
	{
		if(!resource.isLoaded())
			resource.synchronize();

		bool fileExists = FileSystem::isFile(filePath);
		if(fileExists)
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				CM_EXCEPT(InvalidParametersException, "Another file exists at the specified location.");
		}

		mDefaultResourceManifest->registerResource(resource.getUUID(), filePath);

		FileSerializer fs;
		fs.encode(resource.get(), filePath);
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

	HResource Resources::createResourceHandle(const ResourcePtr& obj)
	{
		String uuid = UUIDGenerator::generateRandom();
		HResource newHandle(obj, uuid);

		{
			CM_LOCK_MUTEX(mLoadedResourceMutex);

			mLoadedResources[uuid] = newHandle;
		}
	
		return newHandle;
	}

	bool Resources::getFilePathFromUUID(const String& uuid, WString& filePath) const
	{
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->uuidToFilePath(uuid, filePath))
				return true;
		}

		return false;
	}

	bool Resources::getUUIDFromFilePath(const WString& path, String& uuid) const
	{
		for(auto iter = mResourceManifests.rbegin(); iter != mResourceManifests.rend(); ++iter) 
		{
			if((*iter)->filePathToUUID(path, uuid))
				return true;
		}

		return false;
	}

	void Resources::notifyResourceLoadingFinished(HResource& handle)
	{
		CM_LOCK_MUTEX(mInProgressResourcesMutex);

		mInProgressResources.erase(handle.getUUID());
	}

	void Resources::notifyNewResourceLoaded(HResource& handle)
	{
		CM_LOCK_MUTEX(mLoadedResourceMutex);

		mLoadedResources[handle.getUUID()] = handle;
	}

	CM_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}
}