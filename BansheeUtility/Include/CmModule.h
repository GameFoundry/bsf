#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents one engine module. Essentially it is a specialized type of singleton.
	 * 			Module must be manually started up and shut down before and after use.
	 */
	template <class T>
	class Module
	{
		public:
		/**
		 * @brief	Returns a reference to the module instance. Module has to have been started up
		 * 			first otherwise an exception will be thrown.
		 */
		static T& instance()
		{
			if(isShutDown())
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
		 * @brief	Returns a pointer to the module instance. Module has to have been started up
		 * 			first otherwise an exception will be thrown.
		 */
		static T* instancePtr()
		{
			if (isShutDown())
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
		
		/**
		 * @brief	Constructs and starts the module using the specified parameters.
		 */
		template<class ...Args>
		static void startUp(Args &&...args)
		{
			if (!isShutDown())
				BS_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			_instance() = bs_new<T>(std::forward<Args>(args)...);
			isShutDown() = false;

			((Module*)_instance())->onStartUp();
		}

		/**
		 * @brief	Constructs and starts a specialized type of the module. Provided type
		 *			must derive from type the Module is initialized with.
		 */
		template<class SubType, class ...Args>
		static void startUp(Args &&...args)
		{
			static_assert(std::is_base_of<T, SubType>::value, "Provided type is not derived from type the Module is initialized with.");

			if (!isShutDown())
				BS_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			_instance() = bs_new<SubType>(std::forward<Args>(args)...);
			isShutDown() = false;

			((Module*)_instance())->onStartUp();
		}

		/**
		 * @brief	Shuts down this module and frees any resources it is using.
		 */
		static void shutDown()
		{
			if (isShutDown())
			{
				BS_EXCEPT(InternalErrorException, 
					"Trying to shut down an already shut down module.");
			}

			((Module*)_instance())->onShutDown();

			bs_delete(_instance());
			isShutDown() = true;
		}

		/**
		 * @brief	Query if the module has been started.
		 */
		static bool isStarted()
		{
			return !isShutDown() && !isDestroyed();
		}

	protected:
		Module() 
		{ 
		}

		virtual ~Module()
		{ 
			_instance() = nullptr;
			isDestroyed() = true;
		}

		Module(const Module&) { }
		Module& operator=(const Module&) { return *this; }

		/**
		 * @brief	Override if you want your module to be notified once it has been constructed and started.
		 * 			
		 * @note	Useful when your module is polymorphic and you cannot perform 
		 * 			some implementation specific initialization in constructor itself.
		 */
		virtual void onStartUp() {}

		/**
		 * @brief	Override if you want your module to be notified just before it is deleted.
		 * 			
		 * @note	Useful when your module is polymorphic and you might want to perform some 
		 * 			kind of clean up perhaps overriding that of a base class.
		 */
		virtual void onShutDown() {}

		static T*& _instance()
		{
			static T* inst = nullptr;
			return inst;
		}

		static bool& isDestroyed()
		{
			static bool inst = false;
			return inst;
		}

		static bool& isShutDown()
		{
			static bool inst = true;
			return inst;
		}
	};
}