#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents one engine module. Essentially it is a specialized type of singleton.
	 * 			Module must be manually started up and shut down before and after use.
	 * 			
	 * TODO Low priority - Use variadic templates to automatically pass parameters so I may construct the object instance internally.
	 *			Right now I expect the caller to allocate the object using general memory allocator.
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
			if(isShutDown)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to access a module but it hasn't been started up yet.");
			}

			if(isDestroyed)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to access a destroyed module.");
			}

			return *_instance;
		}

		/**
		 * @brief	Returns a pointer to the module instance. Module has to have been started up
		 * 			first otherwise an exception will be thrown.
		 */
		static T* instancePtr()
		{
			if(isShutDown)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to access a module but it hasn't been started up yet.");
			}

			if(isDestroyed)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to access a destroyed module.");
			}

			return _instance;
		}
		
		/**
		 * @brief	Starts up the module. You must provide an initialized instance of the module,
		 * 			allocated using the general memory allocator.
		 */
		static void startUp(T* inst)
		{
			if(!isShutDown)
				CM_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			_instance = inst;
			isShutDown = false;

			((Module*)_instance)->onStartUp();
		}

		/**
		 * @brief	Shuts down this module and frees any resources it is using.
		 */
		static void shutDown()
		{
			if(isShutDown)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to shut down an already shut down module.");
			}

			((Module*)_instance)->onShutDown();

			cm_delete(_instance);
			isShutDown = true;
		}

		/**
		 * @brief	Query if the module has been started.
		 */
		static bool isStarted()
		{
			return !isShutDown && !isDestroyed;
		}

	protected:
		Module() 
		{ 
		}

		virtual ~Module()
		{ 
			_instance = nullptr;
			isDestroyed = true;
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

		static T* _instance;
		static bool isShutDown;
		static bool isDestroyed;
	};
	
	template <class T>
	T* Module<T>::_instance = nullptr;

	template <class T>
	bool Module<T>::isShutDown = true;

	template <class T>
	bool Module<T>::isDestroyed = false;
}