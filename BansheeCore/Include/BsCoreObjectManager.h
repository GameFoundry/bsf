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
				:destinationObj(nullptr), internalId(0)
			{ }

			CoreStoredSyncObjData(const SPtr<CoreObjectCore> destObj, UINT64 internalId, const CoreSyncData& syncData)
				:destinationObj(destObj), syncData(syncData), internalId(internalId)
			{ }

			SPtr<CoreObjectCore> destinationObj;
			CoreSyncData syncData;
			UINT64 internalId;
		};

		/**
		 * @brief	Stores dirty data that is to be transferred from sim 
		 *			thread to core thread part of a CoreObject, for all dirty
		 *			objects in one frame.
		 */
		struct CoreStoredSyncData
		{
			FrameAlloc* alloc = nullptr;
			Vector<CoreStoredSyncObjData> entries;
		};

		/**
		 * @brief	Contains information about a dirty CoreObject that requires syncing to
		 * 			the core thread.
		 */	
		struct DirtyObjectData
		{
			CoreObject* object;
			INT32 syncDataId;
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
		 * @brief	Notifies the system that a CoreObject is dirty and needs to be synced with the
		 * 			core thread.
		 */
		void notifyCoreDirty(CoreObject* object);

		/**
		 * @brief	Notifies the system that CoreObject dependencies are dirty and should be updated.
		 */
		void notifyDependenciesDirty(CoreObject* object);

		/**
		 * @brief	Synchronizes all dirty CoreObjects with the core thread. Their dirty data will be
		 *			allocated using the global frame allocator and then queued for update using the provided
		 *			core thread accessor.
		 *
		 * @note	Sim thread only.
		 */
		void syncToCore(CoreAccessor& accessor);

		/**
		 * @brief	Synchronizes an individual dirty CoreObject with the core thread. Its dirty data will be
		 *			allocated using the global frame allocator and then queued for update using the provided
		 *			core thread accessor.
		 *
		 * @note	Sim thread only.
		 */
		void syncToCore(CoreObject* object, CoreAccessor& accessor);

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

		/**
		 * @brief	Updates the cached list of dependencies and dependants for the specified object.
		 * 			
		 * @param	object			Update to update dependencies for.
		 * @param	dependencies	New set of dependencies, or null to clear all dependencies.
		 */
		void updateDependencies(CoreObject* object, Vector<CoreObject*>* dependencies);

		UINT64 mNextAvailableID;
		Map<UINT64, CoreObject*> mObjects;
		Map<UINT64, DirtyObjectData> mDirtyObjects;
		Map<UINT64, Vector<CoreObject*>> mDependencies;
		Map<UINT64, Vector<CoreObject*>> mDependants;

		Vector<CoreStoredSyncObjData> mDestroyedSyncData;
		List<CoreStoredSyncData> mCoreSyncData;

		BS_MUTEX(mObjectsMutex);
	};
}