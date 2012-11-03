#include "CmResources.h"
#include "CmResource.h"
#include "CmException.h"
#include "CmFileSerializer.h"
#include "CmFileSystem.h"
#include "CmUUID.h"
#include "CmPath.h"
#include "CmDebug.h"
#include "CmResourcesRTTI.h"

namespace CamelotEngine
{
	bool Resources::ResourceRequestHandler::canHandleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
	{
		return true;
	}

	WorkQueue::Response* Resources::ResourceRequestHandler::handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
	{
		ResourceLoadRequestPtr resRequest = boost::any_cast<ResourceLoadRequestPtr>(req->getData());

		ResourceLoadResponsePtr resResponse = ResourceLoadResponsePtr(new Resources::ResourceLoadResponse());
		resResponse->rawResource = gResources().loadInternal(resRequest->filePath);

		return new WorkQueue::Response(req, true, resResponse);
	}

	bool Resources::ResourceResponseHandler::canHandleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
	{
		return true;
	}

	void Resources::ResourceResponseHandler::handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
	{
		if(res->getRequest()->getAborted())
			return;

		if(res->succeeded())
		{
			ResourceLoadResponsePtr resResponse = boost::any_cast<ResourceLoadResponsePtr>(res->getData());
			ResourceLoadRequestPtr resRequest = boost::any_cast<ResourceLoadRequestPtr>(res->getRequest()->getData());

			resResponse->rawResource->init();
			resRequest->resource.resolve(resResponse->rawResource);
		}
		else
		{
			gDebug().logWarning("Resource load request failed.");
		}
	}

	Resources::Resources(const String& metaDataFolder)
		:mRequestHandler(nullptr), mResponseHandler(nullptr)
	{
		mMetaDataFolderPath = metaDataFolder;

		if(!FileSystem::dirExists(mMetaDataFolderPath))
		{
			FileSystem::createDir(mMetaDataFolderPath);
		}

		loadMetaData();

		mWorkQueue = WorkQueuePtr(new WorkQueue());
		mWorkQueueChannel = mWorkQueue->getChannel("Resources");
		mRequestHandler = new ResourceRequestHandler();
		mResponseHandler = new ResourceResponseHandler();

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
		}

		if(mRequestHandler != nullptr)
			delete mRequestHandler;

		if(mResponseHandler != nullptr)
			delete mResponseHandler;
	}

	void Resources::update()
	{
		mWorkQueue->processResponses();
	}

	BaseResourceRef Resources::load(const String& filePath)
	{
		// TODO - Make sure this uses the same code as loadAsync. Only forceSyncronous

		// TODO - Low priority. Check is file path valid?

		BaseResourceRef resource = loadInternal(filePath);
		resource->init();

		// TODO - This should probably be called in loadInternal, but it isn't thread safe
		//        - loadAsync never calls this code and it should
		if(!metaExists_UUID(resource->getUUID()))
		{
			gDebug().logWarning("Loading a resource that doesn't have meta-data. Creating meta-data automatically. Resource path: " + filePath);
			addMetaData(resource->getUUID(), filePath);
		}

		return resource;
	}

	BaseResourceRef Resources::loadAsync(const String& filePath)
	{
		// TODO - Low priority. Check is file path valid?

		BaseResourceRef newResource;

		ResourceLoadRequestPtr resRequest = ResourceLoadRequestPtr(new Resources::ResourceLoadRequest());
		resRequest->filePath = filePath;
		resRequest->resource = newResource;

		mWorkQueue->addRequest(mWorkQueueChannel, resRequest, 0, false);

		return newResource;
	}

	BaseResourceRef Resources::loadFromUUID(const String& uuid)
	{
		if(!metaExists_UUID(uuid))
		{
			gDebug().logWarning("Cannot load resource. Resource with UUID '" + uuid + "' doesn't exist.");
			return nullptr;
		}

		ResourceMetaDataPtr metaEntry = mResourceMetaData[uuid];

		return load(metaEntry->mPath);
	}

	ResourcePtr Resources::loadInternal(const String& filePath)
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

	void Resources::create(BaseResourceRef resource, const String& filePath, bool overwrite)
	{
		assert(resource.get() != nullptr);

		if(metaExists_UUID(resource->getUUID()))
			CM_EXCEPT(InvalidParametersException, "Specified resource already exists.");
		
		bool fileExists = FileSystem::fileExists(filePath);
		const String& existingUUID = getUUIDFromPath(filePath);
		bool metaExists = metaExists_UUID(existingUUID);

		if(fileExists)
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				CM_EXCEPT(InvalidParametersException, "Another file exists at the specified location.");
		}

		if(metaExists)
			removeMetaData(existingUUID);

		addMetaData(resource->getUUID(), filePath);
		save(resource);
	}

	void Resources::save(BaseResourceRef resource)
	{
		assert(resource.get() != nullptr);

		if(!metaExists_UUID(resource->getUUID()))
			CM_EXCEPT(InvalidParametersException, "Cannot find resource meta-data. Please call Resources::create before trying to save the resource.");

		const String& filePath = getPathFromUUID(resource->getUUID());

		FileSerializer fs;
		fs.encode(resource.get(), filePath);
	}

	void Resources::loadMetaData()
	{
		vector<String>::type allFiles = FileSystem::getFiles(mMetaDataFolderPath);

		for(auto iter = allFiles.begin(); iter != allFiles.end(); ++iter)
		{
			String& path = *iter;
			if(Path::hasExtension(path, "resmeta"))
			{
				FileSerializer fs;
				std::shared_ptr<IReflectable> loadedData = fs.decode(path);

				ResourceMetaDataPtr metaData = std::static_pointer_cast<ResourceMetaData>(loadedData);
				mResourceMetaData[metaData->mUUID] = metaData;
			}
		}
	}

	void Resources::saveMetaData(const ResourceMetaDataPtr metaData)
	{
		String fullPath = Path::combine(mMetaDataFolderPath, metaData->mUUID + ".resmeta");

		FileSerializer fs;
		fs.encode(metaData.get(), fullPath);
	}

	void Resources::removeMetaData(const String& uuid)
	{
		String fullPath = Path::combine(mMetaDataFolderPath, uuid + ".resmeta");
		FileSystem::remove(fullPath);

		mResourceMetaData.erase(uuid);
	}

	void Resources::addMetaData(const String& uuid, const String& filePath)
	{
		if(metaExists_Path(filePath))
			CM_EXCEPT(InvalidParametersException, "Resource with the path '" + filePath + "' already exists.");

		if(metaExists_UUID(uuid))
			CM_EXCEPT(InternalErrorException, "Resource with the same UUID already exists. UUID: " + uuid);

		ResourceMetaDataPtr dbEntry(new ResourceMetaData());
		dbEntry->mPath = filePath;
		dbEntry->mUUID = uuid;

		mResourceMetaData[uuid] = dbEntry;

		saveMetaData(dbEntry);
	}

	void Resources::updateMetaData(const String& uuid, const String& newFilePath)
	{
		if(!metaExists_UUID(uuid))
		{
			CM_EXCEPT(InvalidParametersException, "Cannot update a resource that doesn't exist. UUID: " + uuid + ". File path: " + newFilePath);
		}

		ResourceMetaDataPtr dbEntry = mResourceMetaData[uuid];
		dbEntry->mPath = newFilePath;

		saveMetaData(dbEntry);
	}

	const String& Resources::getPathFromUUID(const String& uuid) const
	{
		auto findIter = mResourceMetaData.find(uuid);

		if(findIter != mResourceMetaData.end())
			return findIter->second->mPath;
		else
			return "";
	}

	const String& Resources::getUUIDFromPath(const String& path) const
	{
		for(auto iter = mResourceMetaData.begin(); iter != mResourceMetaData.end(); ++iter)
		{
			if(iter->second->mPath == path)
			{
				return iter->second->mUUID;
			}
		}

		return StringUtil::BLANK;
	}

	bool Resources::metaExists_UUID(const String& uuid) const
	{
		auto findIter = mResourceMetaData.find(uuid);

		return findIter != mResourceMetaData.end();
	}

	bool Resources::metaExists_Path(const String& path) const
	{
		for(auto iter = mResourceMetaData.begin(); iter != mResourceMetaData.end(); ++iter)
		{
			if(iter->second->mPath == path)
			{
				return true;
			}
		}

		return false;
	}

	CM_EXPORT Resources& gResources()
	{
		return Resources::instance();
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* Resources::ResourceMetaData::getRTTIStatic()
	{
		return ResourceMetaDataRTTI::instance();
	}

	RTTITypeBase* Resources::ResourceMetaData::getRTTI() const
	{
		return Resources::ResourceMetaData::getRTTIStatic();
	}
}