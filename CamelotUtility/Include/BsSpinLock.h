#pragma once

#include <atomic>

namespace BansheeEngine
{
	/**
	 * @brief	Synchronization primitive with low overhead.
	 *
	 * @note	However it will actively block the thread waiting for the lock,
	 *			not allowing any other work to be done, so it is best used for short
	 *			locks.
	 */
	class SpinLock
	{
	public:
		SpinLock()
		{
			mLock.clear(std::memory_order_relaxed);
		}

		/**
		 * @brief	Lock any following operations with the spin lock, not allowing
		 *			any other thread to access them.
		 */
		void lock()
		{
			while(mLock.test_and_set(std::memory_order_acquire))
			{ }
		}

		/**
		 * @brief	Release the lock and allow other threads to acquire the lock.
		 */
		void unlock()
		{
			mLock.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag mLock;
	};

	/**
	 * @brief	Spin lock that is automatically locked upon creation and
	 *			unlocked upon destruction.
	 *
	 * @see		SpinLock
	 */
	class ScopedSpinLock
	{
	public:
		ScopedSpinLock()
		{
			mLock.lock();
		}

		~ScopedSpinLock()
		{
			mLock.unlock();
		}

	private:
		SpinLock mLock;
	};
}