#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"

namespace CamelotFramework
{
	template <class T, bool abstract = false>
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
		
#define MAKE_CM_NEW(z, n, unused)																	\
		template<BOOST_PP_ENUM_PARAMS(n, class T)>													\
		static void startUp(BOOST_PP_ENUM_BINARY_PARAMS(n, T, &&t) )								\
		{																							\
			if(!isShutDown)																			\
				CM_EXCEPT(InternalErrorException, "Trying to start an already started module.");	\
																									\
			_instance = cm_new<T>(std::forward<T0>(t0) BOOST_PP_REPEAT_FROM_TO(1, n, FORWARD_T, ~)); \
			isShutDown = false;																		\
																									\
			((Module*)_instance)->onStartUp();														\
		}

#define FORWARD_T(z, i, unused) \
	, std::forward<BOOST_PP_CAT(T, i)>(BOOST_PP_CAT(t, i))

		BOOST_PP_REPEAT_FROM_TO(1, 15, MAKE_CM_NEW, ~)

#undef FORWARD_T
#undef MAKE_CM_NEW

		static void startUp()
		{
			if(!isShutDown)
				CM_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			_instance = cm_new<T>();
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

	/**
	 * @brief	Represents one engine module. Essentially it is a specialized type of singleton.
	 */
	template <class T>
	class Module<T, true>
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

#define MAKE_CM_NEW(z, n, unused)																		\
		template<class Type BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>									\
		static void startUp(BOOST_PP_ENUM_BINARY_PARAMS(n, T, &&t) )									\
		{																								\
			BOOST_STATIC_ASSERT_MSG((boost::is_base_of<T, Type>::value),								\
				"Invalid type. Specific type must be a child of the type Module is defined with.");		\
																										\
			if(!isShutDown)																				\
				CM_EXCEPT(InternalErrorException, "Trying to start an already started module.");		\
																										\
			_instance = cm_new<Type>(std::forward<T0>(t0) BOOST_PP_REPEAT_FROM_TO(1, n, FORWARD_T, ~));	\
			isShutDown = false;																			\
																										\
			((Module*)_instance)->onStartUp();															\
		}

#define FORWARD_T(z, i, unused) \
	, std::forward<BOOST_PP_CAT(T, i)>(BOOST_PP_CAT(t, i))

			BOOST_PP_REPEAT_FROM_TO(1, 15, MAKE_CM_NEW, ~)

#undef FORWARD_T
#undef MAKE_CM_NEW

		template<class Type>
		static void startUp()
		{
			BOOST_STATIC_ASSERT_MSG((boost::is_base_of<T, Type>::value), 
				"Invalid type. Specific type must be a child of the type Module is defined with.");

			if(!isShutDown)
				CM_EXCEPT(InternalErrorException, "Trying to start an already started module.");

			_instance = cm_new<Type>();
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

	template <class T, bool abstract>
	T* Module<T, abstract>::_instance = nullptr;

	template <class T, bool abstract>
	bool Module<T, abstract>::isShutDown = true;

	template <class T, bool abstract>
	bool Module<T, abstract>::isDestroyed = false;

	template <class T>
	T* Module<T, true>::_instance = nullptr;

	template <class T>
	bool Module<T, true>::isShutDown = true;

	template <class T>
	bool Module<T, true>::isDestroyed = false;
}