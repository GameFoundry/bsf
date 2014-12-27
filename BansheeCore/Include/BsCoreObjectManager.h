#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObjectCore.h"
#include "BsModule.h"

namespace BansheeEngine
{
	// TODO Low priority - Add debug option that would remember a call stack for each resource initialization,
	// so when we fail to release one we know which one it is.
	
	/**
	 * @brief	Manager that keeps track of all active CoreObjects.
	 * 			
	 * @note	Internal class. Thread safe unless specified otherwise.
	 */
	class BS_CORE_EXPORT CoreObjectManager : public Module<CoreObjectManager>
	{
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
		 * @brief	Synchronizes all dirty CoreObjects with the core thread. Their dirty data will be
		 *			allocated using the provided allocator and then queued for update using the provided
		 *			core thread accessor.
		 *
		 * @note	Sim thread only.
		 */
		void syncToCore(CoreAccessor& accessor);

	private:
		/**
		 * @brief	Stores all syncable data from dirty core objects into memory allocated
		 *			by the provided allocator. Additional meta-data is stored internally to be
		 *			used by call to syncUpload.
		 *
		 * @param	allocator Allocator to use for allocating memory for stored data.
		 *
		 * @note	Sim thread only.
		 *			Must be followed by a call to syncUpload with the same type.
		 */
		void syncDownload(FrameAlloc* allocator);

		/**
		 * @brief	Copies all the data stored by previous call to "syncDownload"
		 *			into core thread versions of CoreObjects.
		 *
		 * @note	Core thread only.
		 *			Must be preceded by a call to syncDownload.
		 */
		void syncUpload();

		UINT64 mNextAvailableID;
		Map<UINT64, CoreObject*> mObjects;

		List<CoreStoredSyncData> mCoreSyncData;

		BS_MUTEX(mObjectsMutex);
	};
}