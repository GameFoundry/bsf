#include "BsCoreObjectManager.h"
#include "BsCoreObject.h"
#include "BsCoreObjectCore.h"
#include "BsException.h"
#include "BsMath.h"
#include "BsFrameAlloc.h"
#include "BsCoreThread.h"

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

		for (auto& syncData : mCoreSyncData)
		{
			auto iterFind = std::find_if(syncData.entries.begin(), syncData.entries.end(), 
				[&](const CoreStoredSyncObjData& data) { return data.internalId == internalId; });
			if (iterFind != syncData.entries.end())
			{
				UINT8* data = iterFind->syncData.getBuffer();

				if (data != nullptr && syncData.alloc != nullptr)
					syncData.alloc->dealloc(data);

				syncData.entries.erase(iterFind);
			}
		}
	}

	void CoreObjectManager::syncToCore(CoreAccessor& accessor)
	{
		syncDownload(gCoreThread().getFrameAlloc());
		accessor.queueCommand(std::bind(&CoreObjectManager::syncUpload, this));
	}

	void CoreObjectManager::syncDownload(FrameAlloc* allocator)
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		mCoreSyncData.push_back(CoreStoredSyncData());
		CoreStoredSyncData& syncData = mCoreSyncData.back();

		syncData.alloc = allocator;

		bs_frame_mark();
		
		// Order in which objects are recursed in matters, ones with lower ID will have been created before
		// ones with higher ones and should be updated first.
		for (auto& objectData : mObjects)
		{
			std::function<void(CoreObject*)> syncObject = [&](CoreObject* curObj)
			{
				// Sync dependencies before dependants
				// Note: I don't check for recursion. Possible infinite loop if two objects
				// are dependent on one another.
				
				FrameVector<SPtr<CoreObject>> dependencies;
				curObj->getCoreDependencies(dependencies);

				for (auto& dependency : dependencies)
					syncObject(dependency.get());

				if (curObj->isCoreDirty())
				{
					SPtr<CoreObjectCore> objectCore = curObj->getCore();
					if (objectCore == nullptr)
						return;

					CoreSyncData objSyncData = curObj->syncToCore(allocator);
					curObj->markCoreClean();

					syncData.entries.push_back(CoreStoredSyncObjData(objectCore.get(),
						curObj->getInternalID(), objSyncData));
				}
			};

			CoreObject* object = objectData.second;
			syncObject(object);
		}
		bs_frame_clear();
	}

	void CoreObjectManager::syncUpload()
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (mCoreSyncData.size() == 0)
			return;

		CoreStoredSyncData& syncData = mCoreSyncData.front();

		for (auto& iter = syncData.entries.begin(); iter != syncData.entries.end(); ++iter)
		{
			const CoreStoredSyncObjData& objSyncData = *iter;
			objSyncData.destinationObj->syncToCore(objSyncData.syncData);

			UINT8* data = objSyncData.syncData.getBuffer();

			if (data != nullptr)
				syncData.alloc->dealloc(data);
		}

		syncData.entries.clear();
		mCoreSyncData.pop_front();
	}
}