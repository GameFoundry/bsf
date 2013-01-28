#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	/**
	 * @brief	This class provides some common functionality that all low-level GPU-related objects
	 * 			need to implement.
	 * 			
	 * @note	This involves initializing, keeping track of, and releasing all GPU resources.
	 * 			All core GPU objects are initialized on the render thread, and destroyed on the render thread,
	 * 			so majority of these methods will just schedule object initialization/destruction.
	 */
	class CM_EXPORT CoreGpuObject
	{
	public:
		CoreGpuObject();
		virtual ~CoreGpuObject();

		/**
		 * @brief	Destroys this object. Make sure to call this before deleting the object.
		 * 			
		 * @note	Destruction is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless called from render thread in which case it is executed right away.
		 */
		virtual void destroy();

		/**
		 * @brief	Initializes all the internal resources of this object. Should be called by the
		 * 			factory creation methods automatically after construction and not by user directly.
		 * 					
		 * @note	Initialization is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless called from render thread in which case it is executed right away.
		 */
		virtual void initialize();

		/**
		 * @brief	Returns true if the object has been properly initialized. You are not
		 * 			allowed to call any methods on the resource until you are sure resource is initialized.
		 */
		bool isInitialized() const { return mIsInitialized; }

		/**
		 * @brief	Blocks the current thread until the resource is fully initialized.
		 * 			If you call this without calling initialize first a deadlock will occurr.
		 */
		void waitUntilInitialized();

		/**
		 * @brief	Sets a shared this pointer to this object. This MUST be called immediately after construction.
		 *
		 * @note	Called automatically by the factory creation methods so user should not call this manually.
		 */
		void setThisPtr(std::shared_ptr<CoreGpuObject> ptrThis);

	protected:
		/**
		 * @brief	Frees all of the objects dynamically allocated memory. All derived classes that have something to free
		 * 			should do it here instead of their destructor. All derived classes need to call this base method when they're done.
		 * 			
		 * @note	Since this is scheduled to be executed on the render thread, normally you want to destroy all GPU specific resources here.
		 */
		virtual void destroy_internal();

		/**
		 * @brief	Initializes all the internal resources of this object. Needs to be called before doing
		 * 			any operations with the object. All derived classes also need to call this base method.
		 * 			
		 * @note	Since this is scheduled to be executed on the render thread, normally you want to initialize all GPU specific resources here.
		 */
		virtual void initialize_internal();

		/**
		 * @brief	Returns an unique identifier for this object.
		 */
		UINT64 getInternalID() const { return mInternalID; }

	private:
		friend class CoreGpuObjectManager;

		bool mIsInitialized;
		UINT64 mInternalID; // ID == 0 is not a valid ID
		std::weak_ptr<CoreGpuObject> mThis;

		CM_STATIC_THREAD_SYNCHRONISER(mCoreGpuObjectLoadedCondition)
		CM_STATIC_MUTEX(mCoreGpuObjectLoadedMutex)
	};
}