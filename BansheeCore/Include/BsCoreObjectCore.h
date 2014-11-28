#pragma once

#include "BsCorePrerequisites.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents part of a CoreObject that is meant to be used specifically
	 *			on the core thread. 
	 *
	 * @note	Core thread only.
	 *
	 *			Different CoreObject implementations should implement this class for their
	 *			own needs.
	 */
	class BS_CORE_EXPORT CoreObjectCore
	{
	protected:
		/**
		 * @brief	Values that represent current state of the object
		 */
		enum Flags
		{
			CGCO_INITIALIZED = 0x01, /**< Object has been initialized and can be used. */
			CGCO_SCHEDULED_FOR_INIT = 0x02 /**< Object has been scheduled for initialization but core thread has not completed it yet. */
		};

	public:
		CoreObjectCore();
		virtual ~CoreObjectCore();

		/**
		 * @brief	Called on the core thread when the object is first created.
		 */
		virtual void initialize();

		/**
		 * @brief	Internal method. Sets a shared this pointer to this object. This MUST be called immediately after construction.
		 *
		 * @note	Called automatically by the factory creation methods so user should not call this manually.
		 */
		void _setThisPtr(std::shared_ptr<CoreObjectCore> ptrThis);

		/**
		 * @brief	Returns a shared_ptr version of "this" pointer.
		 */
		SPtr<CoreObjectCore> getThisPtr() const { return mThis.lock(); }

	protected:
		friend class CoreObjectManager;
		friend class CoreObject;

		/**
		 * @brief	Copy internal dirty data to a memory buffer that will be used
		 *			for updating sim thread version of that data.
		 *
		 * @note	This generally happens at the end of a core thread frame. Data is then passed
		 *			to the sim thread and will be available on the next sim thread frame.
		 */
		virtual CoreSyncData syncFromCore(FrameAlloc* allocator) { return CoreSyncData(); }

		/**
		 * @brief	Update internal data from provided memory buffer that
		 *			was populated with data from the sim thread.
		 *
		 * @note	This generally happens at the start of a core thread frame. Data used was
		 *			recorded on the previous sim thread frame.
		 */
		virtual void syncToCore(const CoreSyncData& data) { }

		/**
		 * @brief	Marks the core data as dirty. This causes the syncFromCore()
		 *			method to trigger the next time objects are synced between core and sim threads.
		 *
		 * @param	flags	Optional flags in case you want to signal that only part of the
		 *					internal data is dirty. syncFromCore() will be called regardless
		 *					and it's up to the implementation to read the flags value if needed.
		 */
		void markCoreDirty(UINT32 flags = 0xFFFFFFFF) { mCoreDirtyFlags |= flags; }

		/**
		 * @brief	Marks the core data as clean. Normally called right after syncFromCore()
		 *			has been called.
		 */
		void markCoreClean() { mCoreDirtyFlags = 0; }

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and sim thread potentially needs to be notified.
		 */
		bool isCoreDirty() const { return mCoreDirtyFlags != 0; }

		/**
		 * @brief	Blocks the current thread until the resource is fully initialized.
		 * 			
		 * @note	If you call this without calling initialize first a deadlock will occur.
		 * 			You should not call this from core thread.
		 */
		void synchronize();

		/**
		 * @brief	Returns true if the object has been properly initialized. You are not
		 * 			allowed to call any methods on the object until it is initialized.
		 */
		bool isInitialized() const { return (mFlags & CGCO_INITIALIZED) != 0; }
		bool isScheduledToBeInitialized() const { return (mFlags & CGCO_SCHEDULED_FOR_INIT) != 0; }

		void setIsInitialized(bool initialized) { mFlags = initialized ? mFlags | CGCO_INITIALIZED : mFlags & ~CGCO_INITIALIZED; }
		void setScheduledToBeInitialized(bool scheduled) { mFlags = scheduled ? mFlags | CGCO_SCHEDULED_FOR_INIT : mFlags & ~CGCO_SCHEDULED_FOR_INIT; }

		UINT32 mCoreDirtyFlags;
		volatile UINT8 mFlags;
		std::weak_ptr<CoreObjectCore> mThis;

		BS_STATIC_THREAD_SYNCHRONISER(mCoreGpuObjectLoadedCondition)
		BS_STATIC_MUTEX(mCoreGpuObjectLoadedMutex)
	};
}