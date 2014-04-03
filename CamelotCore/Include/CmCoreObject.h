#pragma once

#include "CmPrerequisites.h"
#include "CmAsyncOp.h"
#include <boost/function.hpp>
#include <boost/preprocessor.hpp>

namespace CamelotFramework
{
	/**
	 * @brief	This class provides some common functionality that all low-level GPU-related objects
	 * 			need to implement.
	 * 			
	 * @note	This involves initializing, keeping track of, and releasing all GPU resources.
	 * 			All core GPU objects are initialized on the core thread, and destroyed on the core thread,
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
		 * 			core thread. Unless called from core thread in which case it is executed right away.
		 */
		virtual void destroy();

		/**
		 * @brief	Initializes all the internal resources of this object. Should be called by the
		 * 			factory creation methods automatically after construction and not by user directly.
		 * 					
		 * @note	Initialization is not done immediately, and is instead just scheduled on the
		 * 			core thread. Unless called from core thread in which case it is executed right away.
		 */
		virtual void initialize();

		/**
		 * @brief	Returns true if the object has been properly initialized. You are not
		 * 			allowed to call any methods on the resource until you are sure resource is initialized.
		 * 			
		 * @note	Normally CPU objects are initialized on creation and this will never be false, and GPU
		 * 			objects are initialized when the Core Thread processes them.
		 */
		bool isInitialized() const { return (mFlags & CGO_INITIALIZED) != 0; }

		/**
		 * @brief	Blocks the current thread until the resource is fully initialized.
		 * 			If you call this without calling initialize first a deadlock will occurr.
		 */
		void synchronize();

		/**
		 * @brief	Sets a shared this pointer to this object. This MUST be called immediately after construction.
		 *
		 * @note	Called automatically by the factory creation methods so user should not call this manually.
		 */
		void setThisPtr(std::shared_ptr<CoreObject> ptrThis);

		/**
		 * @brief	Returns an unique identifier for this object.
		 */
		UINT64 getInternalID() const { return mInternalID; }

		/**
		 * @brief	Schedules the object to be destroyed, and then deleted.
		 *
		 * @note	You should never call this manually. It's meant for internal use only.
		 */
		template<class T, class MemAlloc>
		static void _deleteDelayed(CoreObject* obj)
		{
			_deleteDelayedInternal(obj);

			if(obj->isInitialized())
			{
				std::shared_ptr<CoreObject> thisPtr(obj);
				obj->setThisPtr(thisPtr);
				obj->destroy();
			}
			else
			{
				cm_delete<MemAlloc, T>((T*)obj);
			}
		}

		/**
		 * @brief	Returns a shared_ptr version of "this" pointer.
		 */
		std::shared_ptr<CoreObject> getThisPtr() const { return mThis.lock(); }

	protected:
		/**
		 * @brief	Frees all of the objects dynamically allocated memory. All derived classes that have something to free
		 * 			should do it here instead of their destructor. All derived classes need to call this base method when they're done.
		 * 			
		 * @note	Since this is scheduled to be executed on the core thread, normally you want to destroy all GPU specific resources here.
		 */
		virtual void destroy_internal();

		/**
		 * @brief	Initializes all the internal resources of this object. Needs to be called before doing
		 * 			any operations with the object. All derived classes also need to call this base method.
		 * 			
		 * @note	Since this is scheduled to be executed on the core thread, normally you want to initialize all GPU specific resources here.
		 */
		virtual void initialize_internal();

		static void _deleteDelayedInternal(CoreObject* obj);

		/**
		 * @brief	Queues a command to be executed on the core thread, without a return value.
		 * 			
		 * @note	Requires a shared pointer to the object this function will be executed on, in order to 
		 * 			make sure the object is not deleted before the command executes. Can be null if the 
		 * 			function is static or global.
		 */
		static void queueGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func);

		/**
		 * @brief	Queues a command to be executed on the core thread, with a return value in the form of AsyncOp.
		 * 			
		 * @see		AsyncOp
		 * 			
		 * @note	Requires a shared pointer to the object this function will be executed on, in order to
		 * 			make sure the object is not deleted before the command executes. Can be null if the
		 * 			function is static or global.
		 */
		static AsyncOp queueReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func);

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

		static void queueInitializeGpuCommand(std::shared_ptr<CoreObject>& obj);
		static void queueDestroyGpuCommand(std::shared_ptr<CoreObject>& obj);

		static void executeGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void()> func);
		static void executeReturnGpuCommand(std::shared_ptr<CoreObject>& obj, boost::function<void(AsyncOp&)> func, AsyncOp& op); 
	};

#define MAKE_CM_NEW_CORE(z, n, unused)                                     \
	template<class Type, class MainAlloc, class PtrDataAlloc BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_core_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::shared_ptr<Type>(cm_new<Type, MainAlloc>(BOOST_PP_ENUM_PARAMS (n, t)), &CoreObject::_deleteDelayed<Type, MainAlloc>, StdAlloc<PtrDataAlloc>());     \
	}

	BOOST_PP_REPEAT(15, MAKE_CM_NEW_CORE, ~)

#undef MAKE_CM_NEW_CORE

#define MAKE_CM_NEW_CORE(z, n, unused)                                     \
	template<class Type, class MainAlloc BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_core_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::shared_ptr<Type>(cm_new<Type, MainAlloc>(BOOST_PP_ENUM_PARAMS (n, t)), &CoreObject::_deleteDelayed<Type, MainAlloc>, StdAlloc<GenAlloc>());     \
	}

	BOOST_PP_REPEAT(15, MAKE_CM_NEW_CORE, ~)

#undef MAKE_CM_NEW_CORE

#define MAKE_CM_NEW_CORE(z, n, unused)                                     \
	template<class Type BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>     \
	std::shared_ptr<Type> cm_core_ptr(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t) ) { \
	return std::shared_ptr<Type>(cm_new<Type, GenAlloc>(BOOST_PP_ENUM_PARAMS (n, t)), &CoreObject::_deleteDelayed<Type, GenAlloc>, StdAlloc<GenAlloc>());     \
	}

	BOOST_PP_REPEAT(15, MAKE_CM_NEW_CORE, ~)

#undef MAKE_CM_NEW_CORE

	template<class Type, class MainAlloc>
	std::shared_ptr<Type> cm_core_ptr(Type* data) 
	{
		return std::shared_ptr<Type>(data, &CoreObject::_deleteDelayed<Type, MainAlloc>, StdAlloc<GenAlloc>());  
	}

	template<class Type, class MainAlloc, class PtrDataAlloc>
	std::shared_ptr<Type> cm_core_ptr(Type* data) 
	{
		return std::shared_ptr<Type>(data, &CoreObject::_deleteDelayed<Type, MainAlloc>, StdAlloc<PtrDataAlloc>());  
	}
}