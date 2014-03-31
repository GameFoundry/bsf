#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"

namespace CamelotFramework
{
	/**
	 * @brief	Represents one engine module. Essentially it is a specialized type of singleton.
	 */
	template <class T>
	class Module
	{
		public:
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
		 * @brief	Query if this object has been started.
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

		virtual void onStartUp() {}
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