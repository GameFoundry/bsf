#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"

namespace CamelotEngine
{
	/**
	 * @brief	Represents one engine module. Essentially it is a specialized type of singleton.
	 */
	template <class T> // TODO - Make it thread safe
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

		/**
		 * @brief	Initializes the module.
		 *
		 * @param [in]	moduleInstance Instantiated module. Module takes ownership of instance memory
		 * 								and releases it when shutDown is called.
		 */
		static void startUp(T* moduleInstance)
		{
			if(!isShutDown)
			{
				CM_EXCEPT(InternalErrorException, 
					"Trying to start an already started module.");
			}

			_instance = moduleInstance;
			isShutDown = false;

			moduleInstance->onStartUp();
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

			_instance->onShutDown();

			delete _instance;
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

	private:
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