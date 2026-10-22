//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

namespace b3d
{
	/** @addtogroup Internal-Utility
	 *  @{
	 */

	/** @addtogroup Threading-Internal
	 *  @{
	 */

	namespace detail
	{
		/** No-op mutex for ThreadUnsafe events. Satisfies the BasicLockable concept. */
		struct NullMutex {};

		/** No-op RAII lock guard for ThreadUnsafe events. */
		struct NullLock
		{
			NullLock(NullMutex&) {}
		};
	}

	/** @} */

	/** @addtogroup General-Internal
	 *  @{
	 */

	/** Connection list of an event control block that an event connection is linked into. */
	enum class EventConnectionList : u8
	{
		/** Not linked into any list. Either a deactivated connection still referenced by handles, or a freed one. */
		None,

		/** The list of connections the event calls when triggered. */
		Active,

		/** Connections formed while the event was triggering. They join the active list once the trigger ends. */
		Pending
	};

	/** Data common to all event connections. */
	struct EventConnection
	{
	public:
		EventConnection() = default;

		virtual ~EventConnection()
		{
			B3D_ASSERT(!ActiveHandleCount && !IsActive);
		}

		virtual void Deactivate()
		{
			IsActive = false;
		}

		EventConnection* Previous = nullptr;
		EventConnection* Next = nullptr;
		EventConnectionList List = EventConnectionList::None;
		bool IsActive = true;
		u32 ActiveHandleCount = 0;
	};

	/**
	 * Internal data for an Event, storing all connections. Templated on ThreadSafetyPolicy to conditionally include a mutex.
	 *
	 * While the event is triggering the active list is never modified, so the trigger can walk it while callbacks connect
	 * and disconnect. Connections formed meanwhile go to the pending list and connections released meanwhile only get
	 * deactivated. Both are applied once the outermost trigger ends.
	 */
	template<ThreadSafetyPolicy Policy = ThreadSafe>
	struct TEventControlBlock
	{
		using MutexType = std::conditional_t<Policy == ThreadSafe, RecursiveMutex, detail::NullMutex>;
		using LockType = std::conditional_t<Policy == ThreadSafe, RecursiveLock, detail::NullLock>;

		TEventControlBlock() = default;

		~TEventControlBlock()
		{
			FreeConnectionMemory(ActiveConnections);
			FreeConnectionMemory(PendingConnections);
			FreeConnectionMemory(FreeConnections);
		}

		/** Links a new connection into the active list, or into the pending list if the event is currently triggering. */
		void Connect(EventConnection* connection)
		{
			if(TriggerDepth > 0)
			{
				LinkLast(PendingConnections, LastPendingConnection, connection);
				connection->List = EventConnectionList::Pending;
			}
			else
			{
				LinkLast(ActiveConnections, LastConnection, connection);
				connection->List = EventConnectionList::Active;
			}
		}

		/**
		 * Disconnects the connection with the specified data, ensuring the event doesn't call its callback again.
		 *
		 * @note	Only call this once.
		 */
		void Disconnect(EventConnection* connection)
		{
			LockType lock(Mutex);

			DeactivateConnection(connection);
			connection->ActiveHandleCount--;

			ReleaseConnection(connection);
		}

		/** Disconnects all connections in the event. */
		void DisconnectAll()
		{
			LockType lock(Mutex);

			for(EventConnection* list : { ActiveConnections, PendingConnections })
			{
				EventConnection* connection = list;
				while(connection != nullptr)
				{
					EventConnection* next = connection->Next;

					DeactivateConnection(connection);
					ReleaseConnection(connection);

					connection = next;
				}
			}
		}

		/** Registers a new event handle that references the connection. */
		void AddHandle(EventConnection* connection)
		{
			LockType lock(Mutex);

			connection->ActiveHandleCount++;
		}

		/**
		 * Called when the event handle no longer keeps a reference to the connection data. This means we might be able to
		 * free (and reuse) its memory if the event is done with it too.
		 */
		void FreeHandle(EventConnection* connection)
		{
			LockType lock(Mutex);

			connection->ActiveHandleCount--;

			if(!connection->IsActive)
				ReleaseConnection(connection);
		}

		/** Applies the list changes deferred while the event was triggering. Called once the outermost trigger ends. */
		void EndTrigger()
		{
			if(HasDeferredReleases)
			{
				HasDeferredReleases = false;

				EventConnection* connection = ActiveConnections;
				while(connection != nullptr)
				{
					EventConnection* next = connection->Next;

					if(!connection->IsActive)
					{
						connection->Deactivate();
						ReleaseConnection(connection);
					}

					connection = next;
				}
			}

			if(PendingConnections != nullptr)
			{
				for(EventConnection* connection = PendingConnections; connection != nullptr; connection = connection->Next)
					connection->List = EventConnectionList::Active;

				PendingConnections->Previous = LastConnection;
				if(LastConnection != nullptr)
					LastConnection->Next = PendingConnections;
				else
					ActiveConnections = PendingConnections;

				LastConnection = LastPendingConnection;
				PendingConnections = nullptr;
				LastPendingConnection = nullptr;
			}
		}

		/**
		 * Stops the connection's callback from being called. While the event is triggering, connections in the active list
		 * keep their callback (it might be the one currently executing) until the trigger ends.
		 */
		void DeactivateConnection(EventConnection* connection)
		{
			if(TriggerDepth > 0 && connection->List == EventConnectionList::Active)
			{
				connection->IsActive = false;
				HasDeferredReleases = true;
			}
			else
				connection->Deactivate();
		}

		/**
		 * Unlinks an inactive connection from its list, and frees it if no handles reference it anymore. While the event is
		 * triggering, connections in the active list are left linked until the trigger ends.
		 */
		void ReleaseConnection(EventConnection* connection)
		{
			if(TriggerDepth > 0 && connection->List == EventConnectionList::Active)
				return;

			switch(connection->List)
			{
			case EventConnectionList::Active:
				Unlink(ActiveConnections, LastConnection, connection);
				break;
			case EventConnectionList::Pending:
				Unlink(PendingConnections, LastPendingConnection, connection);
				break;
			case EventConnectionList::None:
				break;
			}

			connection->List = EventConnectionList::None;

			if(connection->ActiveHandleCount > 0)
				return;

			// Freed connections are kept in a singly linked list for re-use by later connections
			connection->Next = FreeConnections;
			FreeConnections = connection;
			connection->~EventConnection();
		}

		/** Appends the connection to the end of the list with the provided first and last entries. */
		static void LinkLast(EventConnection*& first, EventConnection*& last, EventConnection* connection)
		{
			connection->Previous = last;
			connection->Next = nullptr;

			if(last != nullptr)
				last->Next = connection;
			else
				first = connection;

			last = connection;
		}

		/** Removes the connection from the list with the provided first and last entries. */
		static void Unlink(EventConnection*& first, EventConnection*& last, EventConnection* connection)
		{
			if(connection->Previous != nullptr)
				connection->Previous->Next = connection->Next;
			else
				first = connection->Next;

			if(connection->Next != nullptr)
				connection->Next->Previous = connection->Previous;
			else
				last = connection->Previous;

			connection->Previous = nullptr;
			connection->Next = nullptr;
		}

		/** Releases the memory of every connection in the list starting with the provided connection. */
		static void FreeConnectionMemory(EventConnection* connection)
		{
			while(connection != nullptr)
			{
				EventConnection* next = connection->Next;
				B3DFree(connection);

				connection = next;
			}
		}

		EventConnection* ActiveConnections = nullptr;
		EventConnection* LastConnection = nullptr;
		EventConnection* PendingConnections = nullptr;
		EventConnection* LastPendingConnection = nullptr;
		EventConnection* FreeConnections = nullptr;

		MutexType Mutex;

		/** Number of triggers currently executing. Above one when a callback triggers the same event again. */
		u32 TriggerDepth = 0;

		/** True if connections in the active list were deactivated while triggering, and must be released once it ends. */
		bool HasDeferredReleases = false;
	};

	/** @} */
	/** @} */

	/** @addtogroup General
	 *  @{
	 */

	/** Event handle. Allows you to track to which events you subscribed to and disconnect from them when needed. */
	template<ThreadSafetyPolicy Policy = ThreadSafe>
	class THEvent
	{
	public:
		THEvent() = default;

		explicit THEvent(TShared2<TEventControlBlock<Policy>, Policy> eventControlBlock, EventConnection* connection)
			: mConnection(connection), mEventControlBlock(std::move(eventControlBlock))
		{
			connection->ActiveHandleCount++;
		}

		THEvent(const THEvent& other)
			: mConnection(other.mConnection), mEventControlBlock(other.mEventControlBlock)
		{
			if(mConnection != nullptr)
				mEventControlBlock->AddHandle(mConnection);
		}

		THEvent(THEvent&& other) noexcept
			: mConnection(std::exchange(other.mConnection, nullptr)), mEventControlBlock(std::move(other.mEventControlBlock))
		{ }

		~THEvent()
		{
			if(mConnection != nullptr)
				mEventControlBlock->FreeHandle(mConnection);
		}

		/** Disconnect from the event you are subscribed to. */
		void Disconnect()
		{
			if(mConnection != nullptr)
			{
				mEventControlBlock->Disconnect(mConnection);
				mConnection = nullptr;
				mEventControlBlock = nullptr;
			}
		}

		/** @cond IGNORE */

		struct Bool_struct
		{
			int Member;
		};

		/** @endcond */

		/**
		 * Allows direct conversion of a handle to bool.
		 *
		 * @note
		 * Additional struct is needed because we can't directly convert to bool since then we can assign pointer to bool
		 * and that's wrong.
		 */
		operator int Bool_struct::*() const
		{
			return (mConnection != nullptr ? &Bool_struct::Member : 0);
		}

		THEvent& operator=(const THEvent& rhs)
		{
			// Copy first, so self-assignment never releases the last handle of the connection
			THEvent copy(rhs);
			return *this = std::move(copy);
		}

		THEvent& operator=(THEvent&& rhs) noexcept
		{
			if(this != &rhs)
			{
				if(mConnection != nullptr)
					mEventControlBlock->FreeHandle(mConnection);

				mConnection = std::exchange(rhs.mConnection, nullptr);
				mEventControlBlock = std::move(rhs.mEventControlBlock);
			}

			return *this;
		}

	private:
		EventConnection* mConnection = nullptr;
		TShared2<TEventControlBlock<Policy>, Policy> mEventControlBlock;
	};

	/** Alias for thread-safe event handles. */
	using HEvent = THEvent<ThreadSafe>;

	/** @} */

	/** @addtogroup Internal-Utility
	 *  @{
	 */

	/** @addtogroup General-Internal
	 *  @{
	 */

	/**
	 * Events allows you to register method callbacks that get notified when the event is triggered.
	 *
	 * @tparam	Policy			Thread safety policy. ThreadSafe uses a mutex and atomic ref-counting. ThreadUnsafe eliminates
	 *							all locking overhead for use in single-threaded contexts.
	 *
	 * @note	Callback method return value is ignored.
	 */
	template <typename ReturnType, ThreadSafetyPolicy Policy, typename... ArgumentType>
	class TEvent
	{
		struct TEventConnection : EventConnection
		{
			void Deactivate() override
			{
				Callback = nullptr;

				EventConnection::Deactivate();
			}

			std::function<ReturnType(ArgumentType...)> Callback;
		};

		using LockType = typename TEventControlBlock<Policy>::LockType;

	public:
		TEvent()
		{
			// We thread unsafe we allocate control block lazily to avoid overhead of unused events
			if constexpr(Policy == ThreadSafe)
				mControlBlock = B3DMakeShared2<TEventControlBlock<Policy>, Policy>();
		}

		~TEvent()
		{
			Clear();
		}

		/** Register a new callback that will get notified once the event is triggered. */
		THEvent<Policy> Connect(std::function<ReturnType(ArgumentType...)> callback)
		{
			if constexpr(Policy == ThreadUnsafe)
			{
				if(!mControlBlock)
					mControlBlock = B3DMakeShared2<TEventControlBlock<Policy>, Policy>();
			}

			LockType lock(mControlBlock->Mutex);

			TEventConnection* connection = nullptr;
			if(mControlBlock->FreeConnections != nullptr)
			{
				connection = static_cast<TEventConnection*>(mControlBlock->FreeConnections);
				mControlBlock->FreeConnections = connection->Next;

				new(connection) TEventConnection();
			}

			if(connection == nullptr)
				connection = B3DNew<TEventConnection>();

			mControlBlock->Connect(connection);
			connection->Callback = std::move(callback);

			return THEvent<Policy>(mControlBlock, connection);
		}

		/** Trigger the event, notifying all register callback methods. */
		void operator()(ArgumentType... arguments)
		{
			if constexpr(Policy == ThreadUnsafe)
			{
				// Fast path: skip when no connections exist or control block was never allocated.
				// Only safe for ThreadUnsafe events — for ThreadSafe events another thread could Connect() between this check and the lock acquisition.
				if(!mControlBlock || mControlBlock->ActiveConnections == nullptr)
					return;
			}

			// Increase ref count to ensure this event data isn't destroyed if one of the callbacks deletes the event itself.
			TShared2<TEventControlBlock<Policy>, Policy> hoistedControlBlock = mControlBlock;

			LockType lock(hoistedControlBlock->Mutex);
			hoistedControlBlock->TriggerDepth++;

			// The active list isn't modified until the trigger ends, so the next connection stays valid whatever the
			// callback connects or disconnects
			TEventConnection* connection = static_cast<TEventConnection*>(hoistedControlBlock->ActiveConnections);
			while(connection != nullptr)
			{
				TEventConnection* next = static_cast<TEventConnection*>(connection->Next);

				if(connection->IsActive)
					connection->Callback(std::forward<ArgumentType>(arguments)...);

				connection = next;
			}

			hoistedControlBlock->TriggerDepth--;
			if(hoistedControlBlock->TriggerDepth == 0)
				hoistedControlBlock->EndTrigger();
		}

		/** Clear all callbacks from the event. */
		void Clear()
		{
			if(mControlBlock)
				mControlBlock->DisconnectAll();
		}

		/**
		 * Check if event has any callbacks registered.
		 *
		 * @note	It is safe to trigger an event even if no callbacks are registered.
		 */
		bool Empty() const
		{
			if(!mControlBlock)
				return true;

			LockType lock(mControlBlock->Mutex);

			return mControlBlock->ActiveConnections == nullptr && mControlBlock->PendingConnections == nullptr;
		}

	private:
		TShared2<TEventControlBlock<Policy>, Policy> mControlBlock;
	};

	/** @} */
	/** @} */

	/** @addtogroup General
	 *  @{
	 */

	/************************************************************************/
	/* 							SPECIALIZATIONS                      		*/
	/* 	SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE   */
	/************************************************************************/

	/** @copydoc TEvent */
	template <typename Signature, ThreadSafetyPolicy Policy = ThreadSafe>
	class Event;

	/** @copydoc TEvent */
	template <class RetType, class... Args, ThreadSafetyPolicy Policy>
	class Event<RetType(Args...), Policy> : public TEvent<RetType, Policy, Args...>
	{};

	/** @} */
} // namespace b3d
