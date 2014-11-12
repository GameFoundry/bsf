#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObjectCore.h"
#include "BsModule.h"

namespace BansheeEngine
{
	// TODO Low priority - Add debug option that would remember a call stack for each resource initialization,
	// so when we fail to release one we know which one it is.
	
	/**
	 * @brief	Determines type of synchronization to perform when syncing
	 *			dirty data between core and sim threads.
	 */
	enum CoreObjectSync
	{
		Sim, /** Syncing sim thread data to core thread. */
		Core /** Syncing core thread data to sim thread. */
	};

	/**
	 * @brief	Manager that keeps track of all active CoreObjects.
	 * 			
	 * @note	Internal class. Thread safe unless specified otherwise.
	 */
	class BS_CORE_EXPORT CoreObjectManager : public Module<CoreObjectManager>
	{
		/**
		 * @brief	Stores dirty data that is to be transferred from core 
		 *			thread to sim thread part of a CoreObject, for a single object.
		 */
		struct SimStoredSyncObjData
		{
			SimStoredSyncObjData()
				:destinationObj(nullptr)
			{ }

			SimStoredSyncObjData(CoreObject* destObj, const CoreSyncData& syncData)
				:destinationObj(destObj), syncData(syncData)
			{ }

			CoreObject* destinationObj;
			CoreSyncData syncData;
		};

		/**
		 * @brief	Stores dirty data that is to be transferred from sim 
		 *			thread to core thread part of a CoreObject, for a single object.
		 */
		struct CoreStoredSyncObjData
		{
			CoreStoredSyncObjData()
				:destinationObj(nullptr)
			{ }

			CoreStoredSyncObjData(CoreObjectCore* destObj, const CoreSyncData& syncData)
				:destinationObj(destObj), syncData(syncData)
			{ }

			CoreObjectCore* destinationObj;
			CoreSyncData syncData;
		};

		/**
		 * @brief	Stores dirty data that is to be transferred from core 
		 *			thread to sim thread part of a CoreObject, for all dirty 
		 *			objects in one frame.
		 */
		struct SimStoredSyncData
		{
			FrameAlloc* alloc = nullptr;
			Map<UINT64, SimStoredSyncObjData> entries;
		};

		/**
		 * @brief	Stores dirty data that is to be transferred from sim 
		 *			thread to core thread part of a CoreObject, for all dirty
		 *			objects in one frame.
		 */
		struct CoreStoredSyncData
		{
			FrameAlloc* alloc = nullptr;
			Map<UINT64, CoreStoredSyncObjData> entries;
		};

	public:
		CoreObjectManager();
		~CoreObjectManager();

		/**
		 * @brief	Registers a new CoreObject notifying the manager the object
		 * 			is created.
		 */
		UINT64 registerObject(CoreObject* object);

		/**
		 * @brief	Unregisters a CoreObject notifying the manager the object
		 * 			is destroyed.
		 */
		void unregisterObject(CoreObject* object);

		/**
		 * @brief	Stores all syncable data from dirty core objects into memory allocated
		 *			by the provided allocator. Additional meta-data is stored internally to be
		 *			used by call to syncUpload.
		 *
		 * @param	type	Determines where to copy the dirty data from. If set to Sim the data copied
		 *					will be data accessible by the sim thread, and if set to Core the data copied
		 *					will be data accessible by the core thread.
		 * @param	allocator Allocator to use for allocating memory for stored data.
		 *
		 * @note	Not thread safe. If used with Sim type it should only be called from
		 *			sim thread, and if used with Core type it should only be called from
		 *			core thread.
		 *
		 *			Must be followed by a call to syncUpload with the same type.
		 */
		void syncDownload(CoreObjectSync type, FrameAlloc* allocator);

		/**
		 * @brief	Copies all the data stored by previous call to "syncDownload"
		 *			into CoreObjects or their core thread versions.
		 *
		 * @param	type	Determines where to copy the stored data to. This will be the opposite 
		 *					data source than used by "syncDownload". If set to Sim the data will be
		 *					copied to data accessible by core thread, and if set to Core the data 
		 *					will be copied to data accessible by the sim thread. 
		 *
		 * @note	Not thread safe. If used with Sim type it should only be called from
		 *			core thread, and if used with Core type it should only be called from
		 *			sim thread.
		 */
		void syncUpload(CoreObjectSync type);

	private:
		UINT64 mNextAvailableID;
		Map<UINT64, CoreObject*> mObjects;

		Vector<SimStoredSyncData> mSimSyncData;
		UINT32 mSimSyncIdx;
		UINT32 mSimSyncCount;

		Vector<CoreStoredSyncData> mCoreSyncData;
		UINT32 mCoreSyncIdx;
		UINT32 mCoreSyncCount;

		BS_MUTEX(mObjectsMutex);
	};
}