#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 								EVENTS									*/
	/************************************************************************/
	
	class HEvent
	{
	public:
		HEvent(void* connection, void* event, void (*disconnectCallback) (void*, void*))
			:mConnection(connection), mEvent(event), mDisconnectCallback(disconnectCallback)
		{ }

		void disconnect()
		{
			mDisconnectCallback(mConnection, mEvent);
		}

	private:
		void (*mDisconnectCallback) (void*, void*);
		void* mConnection;
		void* mEvent;
	};	

	// 1 parameter
	template <class P0, class RetType>
	class Event1
	{
		struct ConnectionData
		{
		public:
			ConnectionData(std::function<RetType(P0)> func)
				:func(func)
			{ }

			std::function<RetType(P0)> func;
		};

	public:
		~Event1()
		{
			clear();
		}

		HEvent connect(std::function<RetType(P0)> func)
		{
			ConnectionData* connData = cm_new<ConnectionData>(func);

			{
				ScopedSpinLock lock;
				mConnections.push_back(connData);
			}
			
			return HEvent(connData, this, &Event1::disconnectCallback);
		}

		void operator() (P0 args)
		{
			SpinLock lock;

			// TODO - This is TERRIBLE as the spin lock might lock for a long while while
			// the called method is executing. But there is no reason to lock while method is executing.
			// Fix by copying list of connections temporarily.

			// Fix by:
			//  - Using stack alloc allocate N std::function objects
			//  - Create std::function objects using a stack allocator internally

			lock.lock();

			for(auto& connection : mConnections)
			{
				std::function<RetType(P0)> func = connection->func;

				func(args);
			}

			lock.unlock();
		}

		void clear()
		{
			ScopedSpinLock lock;

			for(auto& connection : mConnections)
			{
				cm_delete(connection);
			}

			mConnections.clear();
		}

		bool empty()
		{
			ScopedSpinLock lock;

			return mConnections.size() == 0;
		}

	private:
		friend class EventHandle1;

		typename Vector<ConnectionData*>::type mConnections;

		static void disconnectCallback(void* connection, void* event)
		{
			Event1<P0, RetType>::ConnectionData* castConnection = 
				reinterpret_cast<Event1<P0, RetType>::ConnectionData*>(connection);
			Event1<P0, RetType>* castEvent = reinterpret_cast<Event1<P0, RetType>*>(event);

			castEvent->disconnect(castConnection);
		}

		void disconnect(ConnectionData* connData)
		{
			ScopedSpinLock lock;

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
	template <class P0, class RetType>
	class Event<RetType(P0) > : public Event1 <P0, RetType>
	{ };
}