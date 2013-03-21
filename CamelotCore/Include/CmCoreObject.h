#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include "boost/function.hpp"

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
	class CM_EXPORT CoreObject
	{
	protected:
		enum Flags
		{
			CGO_INITIALIZED = 0x01,
			CGO_INIT_ON_RENDER_THREAD = 0x02,
			CGO_SCHEDULED_FOR_INIT = 0x04,
			CGO_SCHEDULED_FOR_DELETE = 0x08
		};

	public:
		CoreObject(bool requiresGpuInit = true);
		virtual ~CoreObject();

		/**
		 * @brief	Destroys all GPU resources of this object.
o		 * 			
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
		bool isInitialized() const { return (mFlags & CGO_INITIALIZED) != 0; }

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
		void setThisPtr(std::shared_ptr<CoreObject> ptrThis);

		/**
		 * @brief	Schedules the object to be destroyed, and then deleted.
		 *
		 * @note	You should never call this manually. It's meant for internal use only.
		 */
		static void _deleteDelayed(CoreObject* obj);

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
		 * @brief	Returns a shared_ptr version of "this" pointer.
		 */
		std::shared_ptr<CoreObject> getThisPtr() const { return mThis.lock(); }

		/**
		 * @brief	Queues a command to be executed on the render thread, without a return value.
		 * 			
		 * @note	Requires a shared pointer to the object this function will be executed on, in order to 
		 * 			make sure the object is not deleted before the command executes. Can be null if the 
		 * 			function is static or global.
		 */
		static void queueGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func);

		/**
		 * @brief	Queues a command to be executed on the render thread, with a return value in the form of AsyncOp.
		 * 			
		 * @see		AsyncOp
		 * 			
		 * @note	Requires a shared pointer to the object this function will be executed on, in order to
		 * 			make sure the object is not deleted before the command executes. Can be null if the
		 * 			function is static or global.
		 */
		static AsyncOp queueReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func);

		/**
		 * @brief	Returns an unique identifier for this object.
		 */
		UINT64 getInternalID() const { return mInternalID; }

		bool isScheduledToBeInitialized() const { return (mFlags & CGO_SCHEDULED_FOR_INIT) != 0; }
		bool isScheduledToBeDeleted() const { return (mFlags & CGO_SCHEDULED_FOR_DELETE) != 0; }
		bool requiresInitOnRenderThread() const { return (mFlags & CGO_INIT_ON_RENDER_THREAD) != 0; }

		void setIsInitialized(bool initialized) { mFlags = initialized ? mFlags | CGO_INITIALIZED : mFlags & ~CGO_INITIALIZED; }
		void setScheduledToBeInitialized(bool scheduled) { mFlags = scheduled ? mFlags | CGO_SCHEDULED_FOR_INIT : mFlags & ~CGO_SCHEDULED_FOR_INIT; }
		void setScheduledToBeDeleted(bool scheduled) { mFlags = scheduled ? mFlags | CGO_SCHEDULED_FOR_DELETE : mFlags & ~CGO_SCHEDULED_FOR_DELETE; }
	private:
		friend class CoreGpuObjectManager;

		volatile UINT8 mFlags;
		UINT64 mInternalID; // ID == 0 is not a valid ID
		std::weak_ptr<CoreObject> mThis;

		CM_STATIC_THREAD_SYNCHRONISER(mCoreGpuObjectLoadedCondition)
		CM_STATIC_MUTEX(mCoreGpuObjectLoadedMutex)

		static void executeGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func);
		static void executeReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func, AsyncOp& op); 
	};
}