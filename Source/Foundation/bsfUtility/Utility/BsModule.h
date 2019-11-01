//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Error/BsException.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	/**
	 * Represents one engine module. Essentially it is a specialized type of singleton. Module must be manually started up
	 * and shut down before and after use.
	 */
	template <class T>
	class Module
	{
		public:
		/**
		 * Returns a reference to the module instance. Module has to have been started up first otherwise an exception will
		 * be thrown.
		 */
		static T& instance()
		{
			if (!isStartedUp())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to access a module but it hasn't been started up yet.");
			}

			if (isDestroyed())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to access a destroyed module.");
			}

			return *_instance();
		}

		/**
		 * Returns a pointer to the module instance. Module has to have been started up first otherwise an exception will
		 * be thrown.
		 */
		static T* instancePtr()
		{
			if (!isStartedUp())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to access a module but it hasn't been started up yet.");
			}

			if (isDestroyed())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to access a destroyed module.");
			}

			return _instance();
		}

		/** Constructs and starts the module using the specified parameters. */
		template<class ...Args>
		static void startUp(Args &&...args)
		{
			if (isStartedUp())
				BS_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			T * test = nullptr;
			bool const success = _instance().compare_exchange_strong(test, bs_new<T>(std::forward<Args>(args)...));

			if (!success)
				BS_EXCEPT(InternalErrorException, "Race condition when starting module.");

			isStartedUp() = true;

			((Module*)_instance())->onStartUp();
		}

		/**
		 * Constructs and starts a specialized type of the module. Provided type must derive from type the Module is
		 * initialized with.
		 */
		template<class SubType, class ...Args>
		static void startUp(Args &&...args)
		{
			static_assert(std::is_base_of<T, SubType>::value, "Provided type is not derived from type the Module is initialized with.");

			if (isStartedUp())
				BS_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			T * test = nullptr;
			bool const success = _instance().compare_exchange_strong(test, bs_new<SubType>(std::forward<Args>(args)...));

			if (!success)
				BS_EXCEPT(InternalErrorException, "Race condition when starting module.");

			isStartedUp() = true;

			((Module*)_instance())->onStartUp();
		}

		/** Shuts down this module and frees any resources it is using. */
		static void shutDown()
		{
			struct ScopeGuard
			{
				ScopeGuard(Module* inst)
				 : m_inst(inst)
				{ }
				~ScopeGuard()
				{
					bs_delete(inst);
				}
				Module* m_inst;
			};

			if (isDestroyed())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to shut down an already shut down module.");
			}

			if (!isStartedUp())
			{
				BS_EXCEPT(InternalErrorException,
					"Trying to shut down a module which was never started.");
			}

			Module* const inst = _instance().exchange(nullptr);

			if (nullptr == inst)
				BS_EXCEPT(InternalErrorException, "Race condition when starting module.");

			ScopeGuard guard(inst);

			inst->onShutDown();

			isDestroyed() = true;
		}

		/** Query if the module has been started. */
		static bool isStarted()
		{
			return isStartedUp() && !isDestroyed();
		}

	protected:
		Module() = default;

		~Module() = default;

		/*
		 * The notion of copying or moving a singleton is rather nonsensical.
		 */
		Module(Module&&) = delete;
		Module(const Module&) = delete;
		Module& operator=(Module&&) = delete;
		Module& operator=(const Module&) = delete;

		/**
		 * Override if you want your module to be notified once it has been constructed and started.
		 *
		 * @note	Useful when your module is polymorphic and you cannot perform some implementation specific
		 *			initialization in constructor itself.
		 */
		virtual void onStartUp() {}

		/**
		 * Override if you want your module to be notified just before it is deleted.
		 *
		 * @note	Useful when your module is polymorphic and you might want to perform some kind of clean up perhaps
		 *			overriding that of a base class.
		 */
		virtual void onShutDown() {}

		/** Returns a singleton instance of this module. */
		static std::atomic<T*>& _instance()
		{
			static std::atomic<T*> inst{nullptr};
			return inst;
		}

		/**
		 * Checks has the Module been shut down.
		 *
		 * @note	If module was never even started, this will return false.
		 */
		static std::atomic<bool>& isDestroyed()
		{
			static std::atomic<bool> inst{false};
			return inst;
		}

		/** Checks has the Module been started up. */
		static std::atomic<bool>& isStartedUp()
		{
			static std::atomic<bool> inst{false};
			return inst;
		}
	};

	/** @} */
}
