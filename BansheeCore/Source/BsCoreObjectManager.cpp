#include "BsCoreObjectManager.h"
#include "BsCoreObject.h"
#include "BsCoreObjectCore.h"
#include "BsException.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	CoreObjectManager::CoreObjectManager()
		:mNextAvailableID(1), mSimSyncDataAlloc(nullptr), mCoreSyncDataAlloc(nullptr)
	{

	} 

	CoreObjectManager::~CoreObjectManager()
	{
#if BS_DEBUG_MODE
		BS_LOCK_MUTEX(mObjectsMutex);

		if(mObjects.size() > 0)
		{
			// All objects MUST be destroyed at this point, otherwise there might be memory corruption.
			// (Reason: This is called on application shutdown and at that point we also unload any dynamic libraries, 
			// which will invalidate any pointers to objects created from those libraries. Therefore we require of the user to 
			// clean up all objects manually before shutting down the application).
			BS_EXCEPT(InternalErrorException, "Core object manager shut down, but not all objects were released. Application must release ALL " \
				"engine objects before shutdown.");
		}
#endif
	}

	UINT64 CoreObjectManager::registerObject(CoreObject* object)
	{
		assert(object != nullptr);

		BS_LOCK_MUTEX(mObjectsMutex);

		mObjects[mNextAvailableID] = object;

		return mNextAvailableID++;
	}

	void CoreObjectManager::unregisterObject(CoreObject* object)
	{
		assert(object != nullptr);

		BS_LOCK_MUTEX(mObjectsMutex);

		UINT64 internalId = object->getInternalID();
		mObjects.erase(internalId);

		{
			auto iterFind = mSimSyncData.find(internalId);
			if (iterFind != mSimSyncData.end())
			{
				UINT8* data = iterFind->second.syncData.getBuffer();

				if (data != nullptr && mSimSyncDataAlloc != nullptr)
					mSimSyncDataAlloc->dealloc(data);

				mSimSyncData.erase(iterFind);
			}
		}

		{
			auto iterFind = mCoreSyncData.find(internalId);
			if (iterFind != mCoreSyncData.end())
			{
				UINT8* data = iterFind->second.syncData.getBuffer();

				if (data != nullptr && mCoreSyncDataAlloc != nullptr)
					mCoreSyncDataAlloc->dealloc(data);

				mCoreSyncData.erase(iterFind);
			}
		}
	}

	void CoreObjectManager::syncDownload(CoreObjectSync type, FrameAlloc* allocator)
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (type == CoreObjectSync::Sim)
		{
			mCoreSyncDataAlloc = allocator;
			for (auto& objectData : mObjects)
			{
				CoreObject* object = objectData.second;
				SPtr<CoreObjectCore> objectCore = object->getCore();
				if (objectCore != nullptr && object->isCoreDirty())
				{
					CoreSyncData syncData = object->syncToCore(allocator);
					object->markCoreClean();

					mCoreSyncData[object->getInternalID()] = CoreStoredSyncData(objectCore.get(), syncData);
				}
			}
		}
		else
		{
			mSimSyncDataAlloc = allocator;
			for (auto& objectData : mObjects)
			{
				CoreObject* object = objectData.second;
				SPtr<CoreObjectCore> objectCore = object->getCore();
				if (objectCore != nullptr && objectCore->isCoreDirty())
				{
					CoreSyncData syncData = objectCore->syncFromCore(allocator);
					objectCore->markCoreClean();

					mSimSyncData[object->getInternalID()] = SimStoredSyncData(object, syncData);
				}
			}
		}
	}

	void CoreObjectManager::syncUpload(CoreObjectSync type)
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (type == CoreObjectSync::Sim)
		{
			for (auto& objectData : mCoreSyncData)
			{
				const CoreStoredSyncData& syncData = objectData.second;
				syncData.destinationObj->syncToCore(syncData.syncData);

				UINT8* data = syncData.syncData.getBuffer();

				if (data != nullptr)
					mCoreSyncDataAlloc->dealloc(data);
			}

			mCoreSyncData.clear();
		}
		else
		{
			for (auto& objectData : mSimSyncData)
			{
				const SimStoredSyncData& syncData = objectData.second;
				syncData.destinationObj->syncFromCore(syncData.syncData);

				UINT8* data = syncData.syncData.getBuffer();

				if (data != nullptr)
					mSimSyncDataAlloc->dealloc(data);
			}

			mSimSyncData.clear();
		}
	}
}