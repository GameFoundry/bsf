//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "CoreThread/BsCoreObjectCore.h"
#include "Utility/BsModule.h"

namespace bs
{
	/** @addtogroup CoreThread-Internal
	 *  @{
	 */

	// TODO Low priority - Add debug option that would remember a call stack for each resource initialization,
	// so when we fail to release one we know which one it is.
	
	/**
	 * Manager that keeps track of all active CoreObject%s.
	 * 			
	 * @note	Internal class.
	 * @note	Thread safe unless specified otherwise.
	 */
	class BS_CORE_EXPORT CoreObjectManager : public Module<CoreObjectManager>
	{
		/**
		 * Stores dirty data that is to be transferred from sim  thread to core thread part of a CoreObject, for a single
		 * object.
		 */
		struct CoreStoredSyncObjData
		{
			CoreStoredSyncObjData()
				:internalId(0)
			{ }

			CoreStoredSyncObjData(const SPtr<ct::CoreObject> destObj, UINT64 internalId, const CoreSyncData& syncData)
				:destinationObj(destObj), syncData(syncData), internalId(internalId)
			{ }

			SPtr<ct::CoreObject> destinationObj;
			CoreSyncData syncData;
			UINT64 internalId;
		};

		/**
		 * Stores dirty data that is to be transferred from sim thread to core thread part of a CoreObject, for all dirty
		 * objects in one frame.
		 */
		struct CoreStoredSyncData
		{
			FrameAlloc* alloc = nullptr;
			Vector<CoreStoredSyncObjData> entries;
			Vector<SPtr<ct::CoreObject>> destroyedObjects;
		};

		/** Contains information about a dirty CoreObject that requires syncing to the core thread. */	
		struct DirtyObjectData
		{
			CoreObject* object;
			INT32 syncDataId;
		};

	public:
		CoreObjectManager();
		~CoreObjectManager();

		/** Generates a new unique ID for a core object. */
		UINT64 generateId();

		/** Registers a new CoreObject notifying the manager the object	is created. */
		void registerObject(CoreObject* object);

		/** Unregisters a CoreObject notifying the manager the object is destroyed. */
		void unregisterObject(CoreObject* object);

		/**	Notifies the system that a CoreObject is dirty and needs to be synced with the core thread. */
		void notifyCoreDirty(CoreObject* object);

		/**	Notifies the system that CoreObject dependencies are dirty and should be updated. */
		void notifyDependenciesDirty(CoreObject* object);

		/**
		 * Synchronizes all dirty CoreObjects with the core thread. Their dirty data will be allocated using the global
		 * frame allocator and then queued for update using the core thread queue for the calling thread.
		 *
		 * @note	Sim thread only.
		 * @note	This is an @ref asyncMethod "asynchronous method".
		 */
		void syncToCore();

		/**
		 * Synchronizes an individual dirty CoreObject with the core thread. Its dirty data will be allocated using the
		 * global frame allocator and then queued for update the core thread queue for the calling thread.
		 *
		 * @note	Sim thread only.
		 * @note	This is an @ref asyncMethod "asynchronous method".
		 */
		void syncToCore(CoreObject* object);

	private:
		/**
		 * Stores all syncable data from dirty core objects into memory allocated by the provided allocator. Additional
		 * meta-data is stored internally to be used by call to syncUpload().
		 *
		 * @param[in]	allocator Allocator to use for allocating memory for stored data.
		 *
		 * @note	Sim thread only.
		 * @note	Must be followed by a call to syncUpload() with the same type.
		 */
		void syncDownload(FrameAlloc* allocator);

		/**
		 * Copies all the data stored by previous call to syncDownload() into core thread versions of CoreObjects.
		 *
		 * @note	Core thread only.
		 * @note	Must be preceded by a call to syncDownload().
		 */
		void syncUpload();

		/**
		 * Updates the cached list of dependencies and dependants for the specified object.
		 * 			
		 * @param[in]	object			Update to update dependencies for.
		 * @param[in]	dependencies	New set of dependencies, or null to clear all dependencies.
		 */
		void updateDependencies(CoreObject* object, Vector<CoreObject*>* dependencies);

		UINT64 mNextAvailableID;
		Map<UINT64, CoreObject*> mObjects;
		Map<UINT64, DirtyObjectData> mDirtyObjects;
		Map<UINT64, Vector<CoreObject*>> mDependencies;
		Map<UINT64, Vector<CoreObject*>> mDependants;

		Vector<CoreStoredSyncObjData> mDestroyedSyncData;
		List<CoreStoredSyncData> mCoreSyncData;

		Mutex mObjectsMutex;
	};

	/** @} */
}

