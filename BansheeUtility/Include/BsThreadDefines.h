//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#define BS_AUTO_MUTEX_NAME mutex

#if BS_THREAD_SUPPORT

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "BsSpinLock.h"

#define BS_AUTO_MUTEX mutable std::mutex BS_AUTO_MUTEX_NAME;
#define BS_LOCK_AUTO_MUTEX std::unique_lock<std::mutex> bsAutoMutexLock(BS_AUTO_MUTEX_NAME);
#define BS_MUTEX(name) mutable std::mutex name;
#define BS_STATIC_MUTEX(name) static std::mutex name;
#define BS_STATIC_MUTEX_INSTANCE(name) std::mutex name;
#define BS_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName) std::mutex classTypeName##::name;
#define BS_LOCK_MUTEX(name) std::unique_lock<std::mutex> bsnameLock(name);
#define BS_LOCK_MUTEX_NAMED(mutexName, lockName) std::unique_lock<std::mutex> lockName(mutexName);
#define BS_LOCK_TYPE std::unique_lock<std::mutex>
// like BS_AUTO_MUTEX but mutex held by pointer
#define BS_AUTO_SHARED_MUTEX mutable std::mutex *BS_AUTO_MUTEX_NAME;
#define BS_LOCK_AUTO_SHARED_MUTEX assert(BS_AUTO_MUTEX_NAME); std::lock_guard<std::mutex> bsAutoMutexLock(*BS_AUTO_MUTEX_NAME);
#define BS_COPY_AUTO_SHARED_MUTEX(from) assert(!BS_AUTO_MUTEX_NAME); BS_AUTO_MUTEX_NAME = from;
#define BS_SET_AUTO_SHARED_MUTEX_NULL BS_AUTO_MUTEX_NAME = 0;
#define BS_MUTEX_CONDITIONAL(mutex) if (mutex)
#define BS_THREAD_SYNCHRONISER(sync) std::condition_variable sync;
#define BS_STATIC_THREAD_SYNCHRONISER(sync) static std::condition_variable sync;
#define BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName) std::condition_variable classTypeName##::sync;
#define BS_THREAD_WAIT(sync, mutex, lock) sync.wait(lock);
#define BS_THREAD_NOTIFY_ONE(sync) sync.notify_one(); 
#define BS_THREAD_NOTIFY_ALL(sync) sync.notify_all(); 
#define BS_THREAD_JOIN(thread) thread.join();
// Recursive mutex
#define BS_RECURSIVE_MUTEX(name) mutable std::recursive_mutex name
#define BS_LOCK_RECURSIVE_MUTEX(name) std::unique_lock<std::recursive_mutex> cmnameLock(name);
// Read-write mutex
#define BS_RW_MUTEX(name) mutable std::mutex name
#define BS_LOCK_RW_MUTEX_READ(name) std::unique_lock<std::mutex> cmnameLock(name)
#define BS_LOCK_RW_MUTEX_WRITE(name) std::unique_lock<std::mutex> cmnameLock(name)
// Thread objects and related functions
#define BS_THREAD_TYPE std::thread
#define BS_THREAD_CREATE(name, worker) std::thread* name = new (BansheeEngine::MemoryAllocator<BansheeEngine::GenAlloc>::allocate(sizeof(std::thread))) std::thread(worker);
#define BS_THREAD_DESTROY(name) BansheeEngine::bs_delete<BansheeEngine::GenAlloc, std::thread>(name);
#define BS_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()
#define BS_THREAD_CURRENT_ID std::this_thread::get_id()
#define BS_THREAD_ID_TYPE std::thread::id
#define BS_DEFER_LOCK std::defer_lock
#define BS_THREAD_WORKER_INHERIT
// Utility
#define BS_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#else

#define BS_AUTO_MUTEX
#define BS_LOCK_AUTO_MUTEX
#define BS_MUTEX(name)
#define BS_STATIC_MUTEX(name)
#define BS_STATIC_MUTEX_INSTANCE(name)
#define BS_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName)
#define BS_LOCK_MUTEX(name)
#define BS_LOCK_MUTEX_NAMED(mutexName, lockName)
#define BS_LOCK_TYPE UINT32
#define BS_AUTO_SHARED_MUTEX
#define BS_LOCK_AUTO_SHARED_MUTEX
#define BS_COPY_AUTO_SHARED_MUTEX(from)
#define BS_SET_AUTO_SHARED_MUTEX_NULL
#define BS_MUTEX_CONDITIONAL(name) if(true)
#define BS_RW_MUTEX(name)
#define BS_LOCK_RW_MUTEX_READ(name)
#define BS_LOCK_RW_MUTEX_WRITE(name)
#define BS_THREAD_SYNCHRONISER(sync) 
#define BS_STATIC_THREAD_SYNCHRONISER(sync)
#define BS_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName)
#define BS_THREAD_WAIT(sync, lock) 
#define BS_THREAD_NOTIFY_ONE(sync) 
#define BS_THREAD_NOTIFY_ALL(sync) 
#define BS_THREAD_JOIN(thread)
#define BS_THREAD_SLEEP(ms)
#define BS_THREAD_ID_TYPE UINT32
#define BS_THREAD_WORKER_INHERIT
#define BS_DEFER_LOCK

#endif

