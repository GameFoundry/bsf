#include "BsCoreObjectManager.h"
#include "BsCoreObject.h"
#include "BsCoreObjectCore.h"
#include "BsException.h"
#include "BsMath.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	CoreObjectManager::CoreObjectManager()
		:mNextAvailableID(1)
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
			for (auto& syncData : mSimSyncData)
			{
				auto iterFind = syncData.entries.find(internalId);
				if (iterFind != syncData.entries.end())
				{
					UINT8* data = iterFind->second.syncData.getBuffer();

					if (data != nullptr && syncData.alloc != nullptr)
						syncData.alloc->dealloc(data);

					syncData.entries.erase(iterFind);
				}
			}
		}

		{
			for (auto& syncData : mCoreSyncData)
			{
				auto iterFind = syncData.entries.find(internalId);
				if (iterFind != syncData.entries.end())
				{
					UINT8* data = iterFind->second.syncData.getBuffer();

					if (data != nullptr && syncData.alloc != nullptr)
						syncData.alloc->dealloc(data);

					syncData.entries.erase(iterFind);
				}
			}
		}
	}

	void CoreObjectManager::syncDownload(CoreObjectSync type, FrameAlloc* allocator)
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (type == CoreObjectSync::Sim)
		{
			mCoreSyncData.push_back(CoreStoredSyncData());
			CoreStoredSyncData& syncData = mCoreSyncData.back();

			syncData.alloc = allocator;
			for (auto& objectData : mObjects)
			{
				CoreObject* object = objectData.second;
				SPtr<CoreObjectCore> objectCore = object->getCore();
				if (objectCore != nullptr && object->isCoreDirty())
				{
					CoreSyncData objSyncData = object->syncToCore(allocator);
					object->markCoreClean();

					syncData.entries[object->getInternalID()] = CoreStoredSyncObjData(objectCore.get(), objSyncData);
				}
			}
		}
		else
		{
			mSimSyncData.push_back(SimStoredSyncData());
			SimStoredSyncData& syncData = mSimSyncData.back();

			syncData.alloc = allocator;
			for (auto& objectData : mObjects)
			{
				CoreObject* object = objectData.second;
				SPtr<CoreObjectCore> objectCore = object->getCore();
				if (objectCore != nullptr && objectCore->isCoreDirty())
				{
					CoreSyncData objSyncData = objectCore->syncFromCore(allocator);
					objectCore->markCoreClean();

					syncData.entries[object->getInternalID()] = SimStoredSyncObjData(object, objSyncData);
				}
			}
		}
	}

	void CoreObjectManager::syncUpload(CoreObjectSync type)
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (type == CoreObjectSync::Sim)
		{
			if (mCoreSyncData.size() == 0)
				return;

			CoreStoredSyncData& syncData = mCoreSyncData.front();

			for (auto& objectData : syncData.entries)
			{
				const CoreStoredSyncObjData& objSyncData = objectData.second;
				objSyncData.destinationObj->syncToCore(objSyncData.syncData);

				UINT8* data = objSyncData.syncData.getBuffer();

				if (data != nullptr)
					syncData.alloc->dealloc(data);
			}

			syncData.entries.clear();
			mCoreSyncData.pop_front();
		}
		else
		{
			if (mSimSyncData.size() == 0)
				return;

			SimStoredSyncData& syncData = mSimSyncData.front();

			for (auto& objectData : syncData.entries)
			{
				const SimStoredSyncObjData& objSyncData = objectData.second;
				objSyncData.destinationObj->syncFromCore(objSyncData.syncData);

				UINT8* data = objSyncData.syncData.getBuffer();

				if (data != nullptr)
					syncData.alloc->dealloc(data);
			}

			syncData.entries.clear();
			mSimSyncData.pop_front();
		}
	}
}