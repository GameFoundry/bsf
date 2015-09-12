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

		Vector<SPtr<CoreObject>> dependencies;
		Vector<CoreObject*> todo;
		UINT32 stackPos = 0;

		for (auto& objectData : mObjects)
		{
			CoreObject* object = objectData.second;
			
			todo.push_back(object);
			while (stackPos < todo.size())
			{
				CoreObject* curObj = todo[stackPos];
				stackPos++;

				if (curObj->isCoreDirty())
				{
					SPtr<CoreObjectCore> objectCore = curObj->getCore();
					if (objectCore == nullptr)
						continue;

					CoreSyncData objSyncData = curObj->syncToCore(allocator);
					curObj->markCoreClean();

					syncData.entries[curObj->getInternalID()] = CoreStoredSyncObjData(objectCore.get(), objSyncData);
				}

				// Note: I don't check for recursion. Possible infinite loop if two objects
				// are dependent on one another.
				curObj->getCoreDependencies(dependencies);
				for (auto& dependency : dependencies)
					todo.push_back(dependency.get());

				dependencies.clear();
			}

			todo.clear();
			stackPos = 0;
		}
	}

	void CoreObjectManager::syncUpload()
	{
		BS_LOCK_MUTEX(mObjectsMutex);

		if (mCoreSyncData.size() == 0)
			return;

		CoreStoredSyncData& syncData = mCoreSyncData.front();

		// Traverse in reverse to sync dependencies before dependants
		for (auto& riter = syncData.entries.rbegin(); riter != syncData.entries.rend(); ++riter)
		{
			const CoreStoredSyncObjData& objSyncData = riter->second;
			objSyncData.destinationObj->syncToCore(objSyncData.syncData);

			UINT8* data = objSyncData.syncData.getBuffer();

			if (data != nullptr)
				syncData.alloc->dealloc(data);
		}

		syncData.entries.clear();
		mCoreSyncData.pop_front();
	}
}