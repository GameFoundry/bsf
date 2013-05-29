/*-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)

For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/
#pragma once

#define CM_AUTO_MUTEX_NAME mutex

#if CM_THREAD_SUPPORT

// TODO - Port this to std threads so we aren't dependent on boost

#include <boost/thread/tss.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#define CM_AUTO_MUTEX mutable boost::recursive_mutex CM_AUTO_MUTEX_NAME;
#define CM_LOCK_AUTO_MUTEX boost::recursive_mutex::scoped_lock cmAutoMutexLock(CM_AUTO_MUTEX_NAME);
#define CM_MUTEX(name) mutable boost::recursive_mutex name;
#define CM_STATIC_MUTEX(name) static boost::recursive_mutex name;
#define CM_STATIC_MUTEX_INSTANCE(name) boost::recursive_mutex name;
#define CM_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName) boost::recursive_mutex classTypeName##::name;
#define CM_LOCK_MUTEX(name) boost::recursive_mutex::scoped_lock cmnameLock(name);
#define CM_LOCK_MUTEX_NAMED(mutexName, lockName) boost::recursive_mutex::scoped_lock lockName(mutexName);
#define CM_LOCK_TYPE boost::recursive_mutex::scoped_lock
// like CM_AUTO_MUTEX but mutex held by pointer
#define CM_AUTO_SHARED_MUTEX mutable boost::recursive_mutex *CM_AUTO_MUTEX_NAME;
#define CM_LOCK_AUTO_SHARED_MUTEX assert(CM_AUTO_MUTEX_NAME); boost::recursive_mutex::scoped_lock cmAutoMutexLock(*CM_AUTO_MUTEX_NAME);
#define CM_COPY_AUTO_SHARED_MUTEX(from) assert(!CM_AUTO_MUTEX_NAME); CM_AUTO_MUTEX_NAME = from;
#define CM_SET_AUTO_SHARED_MUTEX_NULL CM_AUTO_MUTEX_NAME = 0;
#define CM_MUTEX_CONDITIONAL(mutex) if (mutex)
#define CM_THREAD_SYNCHRONISER(sync) boost::condition sync;
#define CM_STATIC_THREAD_SYNCHRONISER(sync) static boost::condition sync;
#define CM_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName) boost::condition classTypeName##::sync;
#define CM_THREAD_WAIT(sync, mutex, lock) sync.wait(lock);
#define CM_THREAD_NOTIFY_ONE(sync) sync.notify_one(); 
#define CM_THREAD_NOTIFY_ALL(sync) sync.notify_all(); 
// Read-write mutex
#define CM_RW_MUTEX(name) mutable boost::shared_mutex name
#define CM_LOCK_RW_MUTEX_READ(name) boost::shared_lock<boost::shared_mutex> cmnameLock(name)
#define CM_LOCK_RW_MUTEX_WRITE(name) boost::unique_lock<boost::shared_mutex> cmnameLock(name)
// Thread objects and related functions
#define CM_THREAD_TYPE boost::thread
#define CM_THREAD_CREATE(name, worker) boost::thread* name = new (CamelotFramework::MemoryAllocator<CamelotFramework::GenAlloc>::allocate(sizeof(boost::thread))) boost::thread(worker);
#define CM_THREAD_DESTROY(name) CamelotFramework::cm_delete<CamelotFramework::GenAlloc, boost::thread>(name);
#define CM_THREAD_HARDWARE_CONCURRENCY boost::thread::hardware_concurrency()
#define CM_THREAD_CURRENT_ID boost::this_thread::get_id()
#define CM_THREAD_ID_TYPE boost::thread::id
#define CM_THREAD_WORKER_INHERIT
// Utility
#define CM_THREAD_SLEEP(ms) boost::this_thread::sleep(boost::posix_time::millisec(ms));

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

#endif

