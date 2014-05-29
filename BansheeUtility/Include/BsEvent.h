#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Data common to all event connections.
	 */
	class BaseConnectionData
	{
	public:
		bool isValid;
	};

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

		HEvent(std::shared_ptr<BaseConnectionData> connection, void* event, void(*disconnectCallback) (const std::shared_ptr<BaseConnectionData>&, void*))
			:mConnection(connection), mEvent(event), mDisconnectCallback(disconnectCallback)
		{ }

		/**
		 * @brief	Disconnect from the event you are subscribed to.
		 *
		 * @note	Caller must ensure the event is still valid.
		 */
		void disconnect()
		{
			if (mConnection != nullptr && mConnection->isValid)
				mDisconnectCallback(mConnection, mEvent);
		}

	private:
		void(*mDisconnectCallback) (const std::shared_ptr<BaseConnectionData>&, void*);
		std::shared_ptr<BaseConnectionData> mConnection;
		void* mEvent;
	};	

	/**
	 * @brief	Events allows you to register method callbacks that get notified
	 *			when the event is triggered.
	 *
	 * @note	Callback method return value is ignored.
	 */
	template <class RetType, class... Args>
	class TEvent
	{
		struct ConnectionData : BaseConnectionData
		{
		public:
			ConnectionData(std::function<RetType(Args...)> func)
				:func(func)
			{ }

			std::function<RetType(Args...)> func;
		};

	public:
		TEvent()
			:mHasDisconnectedCallbacks(false)
		{ }

		~TEvent()
		{
			clear();
		}

		/**
		 * @brief	Register a new callback that will get notified once
		 *			the event is triggered.
		 */
		HEvent connect(std::function<RetType(Args...)> func)
		{
			std::shared_ptr<ConnectionData> connData = bs_shared_ptr<ConnectionData>(func);
			connData->isValid = true;

			{
				BS_LOCK_RECURSIVE_MUTEX(mMutex);
				mConnections.push_back(connData);
			}
			
			return HEvent(connData, this, &TEvent::disconnectCallback);
		}

		/**
		 * @brief	Trigger the event, notifying all register callback methods.
		 */
		void operator() (Args... args)
		{
			BS_LOCK_RECURSIVE_MUTEX(mMutex);

			// Here is the only place we remove connections, in order to allow disconnect() and clear() to be called
			// recursively from the notify callbacks
			if (mHasDisconnectedCallbacks)
			{
				for (UINT32 i = 0; i < mConnections.size(); i++)
				{
					if (!mConnections[i]->isValid)
					{
						mConnections.erase(mConnections.begin() + i);
						i--;
					}
				}

				mHasDisconnectedCallbacks = false;
			}

			// Do not use an iterator here, as new connections might be added during iteration from
			// the notify callback
			UINT32 numConnections = (UINT32)mConnections.size(); // Remember current num. connections as we don't want to notify new ones
			for (UINT32 i = 0; i < numConnections; i++)
				mConnections[i]->func(args...);
		}

		/**
		 * @brief	Clear all callbacks from the event.
		 */
		void clear()
		{
			BS_LOCK_RECURSIVE_MUTEX(mMutex);

			for(auto& connection : mConnections)
				connection->isValid = false;

			if (mConnections.size() > 0)
				mHasDisconnectedCallbacks = true;
		}

		/**
		 * @brief	Check if event has any callbacks registered.
		 *
		 * @note	It is safe to trigger an event even if no callbacks are registered.
		 */
		bool empty()
		{
			BS_LOCK_RECURSIVE_MUTEX(mMutex);

			return mConnections.size() == 0;
		}

	private:
		Vector<std::shared_ptr<ConnectionData>> mConnections;
		bool mHasDisconnectedCallbacks;
		BS_RECURSIVE_MUTEX(mMutex);

		/**
		 * @brief	Callback triggered by event handles when they want to disconnect from
		 *			an event.
		 */
		static void disconnectCallback(const std::shared_ptr<BaseConnectionData>& connection, void* event)
		{
			TEvent<RetType, Args...>* castEvent = reinterpret_cast<TEvent<RetType, Args...>*>(event);

			castEvent->disconnect(connection);
		}

		/**
		 * @brief	Internal method that disconnects the callback described by the provided connection data.
		 */
		void disconnect(const std::shared_ptr<BaseConnectionData>& connData)
		{
			BS_LOCK_RECURSIVE_MUTEX(mMutex);

			for(auto& iter = mConnections.begin(); iter != mConnections.end(); ++iter)
			{
				if((*iter) == connData)
				{
					connData->isValid = false;
					mHasDisconnectedCallbacks = true;
					return;
				}
			}
		}
	};

	/************************************************************************/
	/* 							SPECIALIZATIONS                      		*/
	/* 	SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE   */
	/************************************************************************/
	
	/**
	 * @copydoc	TEvent
	 */
	template <typename Signature>
	class Event;

	/**
	* @copydoc	TEvent
	*/
	template <class RetType, class... Args>
	class Event<RetType(Args...) > : public TEvent <RetType, Args...>
	{ };
}