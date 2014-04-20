#pragma once

#define CM_AUTO_MUTEX_NAME mutex

#if CM_THREAD_SUPPORT

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#define CM_AUTO_MUTEX mutable std::mutex CM_AUTO_MUTEX_NAME;
#define CM_LOCK_AUTO_MUTEX std::unique_lock<std::mutex> cmAutoMutexLock(CM_AUTO_MUTEX_NAME);
#define CM_MUTEX(name) mutable std::mutex name;
#define CM_STATIC_MUTEX(name) static std::mutex name;
#define CM_STATIC_MUTEX_INSTANCE(name) std::mutex name;
#define CM_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName) std::mutex classTypeName##::name;
#define CM_LOCK_MUTEX(name) std::unique_lock<std::mutex> cmnameLock(name);
#define CM_LOCK_MUTEX_NAMED(mutexName, lockName) std::unique_lock<std::mutex> lockName(mutexName);
#define CM_LOCK_TYPE std::unique_lock<std::mutex>
// like CM_AUTO_MUTEX but mutex held by pointer
#define CM_AUTO_SHARED_MUTEX mutable std::mutex *CM_AUTO_MUTEX_NAME;
#define CM_LOCK_AUTO_SHARED_MUTEX assert(CM_AUTO_MUTEX_NAME); std::lock_guard<std::mutex> cmAutoMutexLock(*CM_AUTO_MUTEX_NAME);
#define CM_COPY_AUTO_SHARED_MUTEX(from) assert(!CM_AUTO_MUTEX_NAME); CM_AUTO_MUTEX_NAME = from;
#define CM_SET_AUTO_SHARED_MUTEX_NULL CM_AUTO_MUTEX_NAME = 0;
#define CM_MUTEX_CONDITIONAL(mutex) if (mutex)
#define CM_THREAD_SYNCHRONISER(sync) std::condition_variable sync;
#define CM_STATIC_THREAD_SYNCHRONISER(sync) static std::condition_variable sync;
#define CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName) std::condition_variable classTypeName##::sync;
#define CM_THREAD_WAIT(sync, mutex, lock) sync.wait(lock);
#define CM_THREAD_NOTIFY_ONE(sync) sync.notify_one(); 
#define CM_THREAD_NOTIFY_ALL(sync) sync.notify_all(); 
// Read-write mutex
#define CM_RW_MUTEX(name) mutable std::mutex name
#define CM_LOCK_RW_MUTEX_READ(name) std::unique_lock<std::mutex> cmnameLock(name)
#define CM_LOCK_RW_MUTEX_WRITE(name) std::unique_lock<std::mutex> cmnameLock(name)
// Thread objects and related functions
#define CM_THREAD_TYPE std::thread
#define CM_THREAD_CREATE(name, worker) std::thread* name = new (BansheeEngine::MemoryAllocator<BansheeEngine::GenAlloc>::allocate(sizeof(std::thread))) std::thread(worker);
#define CM_THREAD_DESTROY(name) BansheeEngine::cm_delete<BansheeEngine::GenAlloc, std::thread>(name);
#define CM_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()
#define CM_THREAD_CURRENT_ID std::this_thread::get_id()
#define CM_THREAD_ID_TYPE std::thread::id
#define CM_DEFER_LOCK std::defer_lock
#define CM_THREAD_WORKER_INHERIT
// Utility
#define CM_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#else

#define CM_AUTO_MUTEX
#define CM_LOCK_AUTO_MUTEX
#define CM_MUTEX(name)
#define CM_STATIC_MUTEX(name)
#define CM_STATIC_MUTEX_INSTANCE(name)
#define CM_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName)
#define CM_LOCK_MUTEX(name)
#define CM_LOCK_MUTEX_NAMED(mutexName, lockName)
#define CM_LOCK_TYPE UINT32
#define CM_AUTO_SHARED_MUTEX
#define CM_LOCK_AUTO_SHARED_MUTEX
#define CM_COPY_AUTO_SHARED_MUTEX(from)
#define CM_SET_AUTO_SHARED_MUTEX_NULL
#define CM_MUTEX_CONDITIONAL(name) if(true)
#define CM_RW_MUTEX(name)
#define CM_LOCK_RW_MUTEX_READ(name)
#define CM_LOCK_RW_MUTEX_WRITE(name)
#define CM_THREAD_SYNCHRONISER(sync) 
#define CM_STATIC_THREAD_SYNCHRONISER(sync)
#define CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName)
#define CM_THREAD_WAIT(sync, lock) 
#define CM_THREAD_NOTIFY_ONE(sync) 
#define CM_THREAD_NOTIFY_ALL(sync) 
#define CM_THREAD_SLEEP(ms)
#define CM_THREAD_ID_TYPE UINT32
#define CM_THREAD_WORKER_INHERIT
#define CM_DEFER_LOCK

#endif

