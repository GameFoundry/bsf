#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 								EVENTS									*/
	/************************************************************************/
	
	/**
	 * @brief	Event handle. Allows you to track to which events you subscribed to and
	 *			disconnect from them when needed.
	 */
	class HEvent
	{
	public:
		HEvent()
			:mDisconnectCallback(nullptr), mConnection(nullptr), mEvent(nullptr)
		{ }

		HEvent(void* connection, void* event, void (*disconnectCallback) (void*, void*))
			:mConnection(connection), mEvent(event), mDisconnectCallback(disconnectCallback)
		{ }

		/**
		 * @brief	Disconnect from the event you are subscribed to.
		 *
		 * @note	Caller must ensure the event is still valid.
		 */
		void disconnect()
		{
			if (mDisconnectCallback != nullptr)
				mDisconnectCallback(mConnection, mEvent);

			mDisconnectCallback = nullptr;
		}

	private:
		void (*mDisconnectCallback) (void*, void*);
		void* mConnection;
		void* mEvent;
	};	

	template <class RetType, class... Args>
	class TEvent
	{
		struct ConnectionData
		{
		public:
			ConnectionData(std::function<RetType(Args...)> func)
				:func(func)
			{ }

			std::function<RetType(Args...)> func;
		};

	public:
		~TEvent()
		{
			clear();
		}

		HEvent connect(std::function<RetType(Args...)> func)
		{
			ConnectionData* connData = cm_new<ConnectionData>(func);

			{
				CM_LOCK_MUTEX(mMutex);
				mConnections.push_back(connData);
			}
			
			return HEvent(connData, this, &TEvent::disconnectCallback);
		}

		void operator() (Args... args)
		{
			CM_LOCK_MUTEX(mMutex);

			for(auto& connection : mConnections)
			{
				std::function<RetType(Args...)> func = connection->func;

				func(args...);
			}
		}

		void clear()
		{
			CM_LOCK_MUTEX(mMutex);

			for(auto& connection : mConnections)
			{
				cm_delete(connection);
			}

			mConnections.clear();
		}

		bool empty()
		{
			CM_LOCK_MUTEX(mMutex);

			return mConnections.size() == 0;
		}

	private:
		typename Vector<ConnectionData*>::type mConnections;
		CM_MUTEX(mMutex);

		static void disconnectCallback(void* connection, void* event)
		{
			TEvent<RetType, Args...>::ConnectionData* castConnection =
				reinterpret_cast<TEvent<RetType, Args...>::ConnectionData*>(connection);
			TEvent<RetType, Args...>* castEvent = reinterpret_cast<TEvent<RetType, Args...>*>(event);

			castEvent->disconnect(castConnection);
		}

		void disconnect(ConnectionData* connData)
		{
			CM_LOCK_MUTEX(mMutex);

			for(auto& iter = mConnections.begin(); iter != mConnections.end(); ++iter)
			{
				if((*iter) == connData)
				{
					cm_delete(connData);
					mConnections.erase(iter);
					return;
				}
			}
		}
	};

	/************************************************************************/
	/* 							SPECIALIZATIONS                      		*/
	/* 	SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE   */
	/************************************************************************/
	
	template <typename Signature>
	class Event;

	// 1 parameter
	template <class RetType, class... Args>
	class Event<RetType(Args...) > : public TEvent <RetType, Args...>
	{ };
}