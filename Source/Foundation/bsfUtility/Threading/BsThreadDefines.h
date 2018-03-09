//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#define BS_AUTO_MUTEX_NAME mutex

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Threading/BsSpinLock.h"

/** @addtogroup Threading
 *  @{
 */

/** Returns the number of logical CPU cores. */
#define BS_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()

/** Returns the ThreadId of the current thread. */
#define BS_THREAD_CURRENT_ID std::this_thread::get_id()

/** Causes the current thread to sleep for the provided amount of milliseconds. */
#define BS_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

/** Wrapper for the C++ std::mutex. */
using Mutex = std::mutex;

/** Wrapper for the C++ std::recursive_mutex. */
using RecursiveMutex = std::recursive_mutex;

/** Wrapper for the C++ std::condition_variable. */
using Signal = std::condition_variable;

/** Wrapper for the C++ std::thread. */
using Thread = std::thread;

/** Wrapper for the C++ std::thread::id. */
using ThreadId = std::thread::id;

/** Wrapper for the C++ std::unique_lock<std::mutex>. */
using Lock = std::unique_lock<Mutex>;

/** Wrapper for the C++ std::unique_lock<std::recursive_mutex>. */
using RecursiveLock = std::unique_lock<RecursiveMutex>;

/** @} */