/*
-----------------------------------------------------------------------------
This source file is part of OGRE
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
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmWorkQueue.h"
#include "CmDebug.h"

namespace CamelotFramework {
	WorkQueue::WorkQueue() 
		: mNextChannel(0) 		
		, mWorkerThreadCount(1)
		, mIsRunning(false)
		, mWorkerFunc(0)
		, mRequestCount(0)
		, mPaused(false)
		, mAcceptRequests(true)
	{}
	//---------------------------------------------------------------------
	WorkQueue::~WorkQueue() 
	{
		shutdown();

		for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
		{
			CM_DELETE(*i, Request, ScratchAlloc);
		}
		mRequestQueue.clear();
	}
	//---------------------------------------------------------------------
	void WorkQueue::startup(bool forceRestart)
	{
		if (mIsRunning)
		{
			if (forceRestart)
				shutdown();
			else
				return;
		}

		mShuttingDown = false;

		mWorkerFunc = CM_NEW(WorkerFunc, GenAlloc) WorkerFunc(this);

#if CM_THREAD_SUPPORT
		for (UINT8 i = 0; i < mWorkerThreadCount; ++i)
		{
			CM_THREAD_CREATE(t, *mWorkerFunc);
			mWorkers.push_back(t);
		}
#endif

		mIsRunning = true;
	}
	//---------------------------------------------------------------------
	void WorkQueue::shutdown()
	{
		if( !mIsRunning )
			return;

		mShuttingDown = true;
		abortAllRequests();
#if CM_THREAD_SUPPORT
		// wake all threads (they should check shutting down as first thing after wait)
		CM_THREAD_NOTIFY_ALL(mRequestCondition)

			// all our threads should have been woken now, so join
			for (WorkerThreadList::iterator i = mWorkers.begin(); i != mWorkers.end(); ++i)
			{
				(*i)->join();
				CM_THREAD_DESTROY(*i);
			}
			mWorkers.clear();
#endif

			if (mWorkerFunc != nullptr)
			{
				CM_DELETE(mWorkerFunc, WorkerFunc, GenAlloc);
				mWorkerFunc = nullptr;
			}

			mIsRunning = false;
	}
	//---------------------------------------------------------------------
	void WorkQueue::addRequestHandler(UINT16 channel, RequestHandler* rh)
	{
		CM_LOCK_RW_MUTEX_WRITE(mRequestHandlerMutex);

		RequestHandlerListByChannel::iterator i = mRequestHandlers.find(channel);
		if (i == mRequestHandlers.end())
			i = mRequestHandlers.insert(RequestHandlerListByChannel::value_type(channel, RequestHandlerList())).first;

		RequestHandlerList& handlers = i->second;
		bool duplicate = false;
		for (RequestHandlerList::iterator j = handlers.begin(); j != handlers.end(); ++j)
		{
			if ((*j)->getHandler() == rh)
			{
				duplicate = true;
				break;
			}
		}
		if (!duplicate)
			handlers.push_back(RequestHandlerHolderPtr(CM_NEW(RequestHandlerHolder, GenAlloc) RequestHandlerHolder(rh),
				&MemAllocDeleter<RequestHandlerHolder, GenAlloc>::deleter));

	}
	//---------------------------------------------------------------------
	void WorkQueue::removeRequestHandler(UINT16 channel, RequestHandler* rh)
	{
		CM_LOCK_RW_MUTEX_WRITE(mRequestHandlerMutex);

		RequestHandlerListByChannel::iterator i = mRequestHandlers.find(channel);
		if (i != mRequestHandlers.end())
		{
			RequestHandlerList& handlers = i->second;
			for (RequestHandlerList::iterator j = handlers.begin(); j != handlers.end(); ++j)
			{
				if ((*j)->getHandler() == rh)
				{
					// Disconnect - this will make it safe across copies of the list
					// this is threadsafe and will wait for existing processes to finish
					(*j)->disconnectHandler();
					handlers.erase(j);	
					break;
				}
			}

		}

	}
	//---------------------------------------------------------------------
	void WorkQueue::addResponseHandler(UINT16 channel, ResponseHandler* rh)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(channel);
		if (i == mResponseHandlers.end())
			i = mResponseHandlers.insert(ResponseHandlerListByChannel::value_type(channel, ResponseHandlerList())).first;

		ResponseHandlerList& handlers = i->second;
		if (std::find(handlers.begin(), handlers.end(), rh) == handlers.end())
			handlers.push_back(rh);
	}
	//---------------------------------------------------------------------
	void WorkQueue::removeResponseHandler(UINT16 channel, ResponseHandler* rh)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(channel);
		if (i != mResponseHandlers.end())
		{
			ResponseHandlerList& handlers = i->second;
			ResponseHandlerList::iterator j = std::find(
				handlers.begin(), handlers.end(), rh);
			if (j != handlers.end())
				handlers.erase(j);

		}
	}
	//---------------------------------------------------------------------
	WorkQueue::RequestID WorkQueue::peekNextFreeRequestId()
	{
		{
			// lock to acquire rid and push request to the queue
			CM_LOCK_MUTEX(mRequestMutex)

			RequestID rid = mRequestCount + 1;
			return rid;
		}
	}
	//---------------------------------------------------------------------
	WorkQueue::RequestID WorkQueue::addRequest(UINT16 channel, 
		const boost::any& rData, UINT8 retryCount, bool forceSynchronous)
	{
		Request* req = 0;
		RequestID rid = 0;

		{
			// lock to acquire rid and push request to the queue
			CM_LOCK_MUTEX(mRequestMutex)

				if (!mAcceptRequests || mShuttingDown)
					return 0;

			rid = ++mRequestCount;
			req = CM_NEW(Request, ScratchAlloc) Request(channel, rData, retryCount, rid);

#if CM_THREAD_SUPPORT
			if (!forceSynchronous)
			{
				mRequestQueue.push_back(req);
				notifyWorkers();
				return rid;
			}
#endif
		}

		processRequestResponse(req);

		return rid;

	}
	//---------------------------------------------------------------------
	void WorkQueue::addRequestWithRID(WorkQueue::RequestID rid, UINT16 channel, 
		const boost::any& rData, UINT8 retryCount)
	{
		// lock to push request to the queue
		CM_LOCK_MUTEX(mRequestMutex)

		if (mShuttingDown)
			return;

		Request* req = CM_NEW(Request, ScratchAlloc) Request(channel, rData, retryCount, rid);

#if CM_THREAD_SUPPORT
		mRequestQueue.push_back(req);
		notifyWorkers();
#else
		processRequestResponse(req);
#endif
	}
	//---------------------------------------------------------------------
	void WorkQueue::abortRequest(RequestID id)
	{
		CM_LOCK_MUTEX(mProcessMutex)

			// NOTE: Pending requests are exist any of RequestQueue, ProcessQueue and
			// ResponseQueue when keeping ProcessMutex, so we check all of these queues.

			for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
			{
				if ((*i)->getID() == id)
				{
					(*i)->abortRequest();
					break;
				}
			}

			{
				CM_LOCK_MUTEX(mRequestMutex)

					for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
					{
						if ((*i)->getID() == id)
						{
							(*i)->abortRequest();
							break;
						}
					}
			}
	}
	//---------------------------------------------------------------------
	void WorkQueue::abortRequestsByChannel(UINT16 channel)
	{
		CM_LOCK_MUTEX(mProcessMutex)

			for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
			{
				if ((*i)->getChannel() == channel)
				{
					(*i)->abortRequest();
				}
			}

			{
				CM_LOCK_MUTEX(mRequestMutex)

					for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
					{
						if ((*i)->getChannel() == channel)
						{
							(*i)->abortRequest();
						}
					}
			}
	}
	//---------------------------------------------------------------------
	void WorkQueue::abortAllRequests()
	{
		CM_LOCK_MUTEX(mProcessMutex)

			for (RequestQueue::iterator i = mProcessQueue.begin(); i != mProcessQueue.end(); ++i)
			{
				(*i)->abortRequest();
			}

			{
				CM_LOCK_MUTEX(mRequestMutex)

					for (RequestQueue::iterator i = mRequestQueue.begin(); i != mRequestQueue.end(); ++i)
					{
						(*i)->abortRequest();
					}
			}
	}
	//---------------------------------------------------------------------
	void WorkQueue::setPaused(bool pause)
	{
		CM_LOCK_MUTEX(mRequestMutex)

		mPaused = pause;
	}
	//---------------------------------------------------------------------
	bool WorkQueue::isPaused() const
	{
		return mPaused;
	}
	//---------------------------------------------------------------------
	void WorkQueue::setRequestsAccepted(bool accept)
	{
		CM_LOCK_MUTEX(mRequestMutex)

		mAcceptRequests = accept;
	}
	//---------------------------------------------------------------------
	bool WorkQueue::getRequestsAccepted() const
	{
		return mAcceptRequests;
	}
	//---------------------------------------------------------------------
	size_t WorkQueue::getWorkerThreadCount() const
	{
		return mWorkerThreadCount;
	}
	//---------------------------------------------------------------------
	void WorkQueue::setWorkerThreadCount(size_t c)
	{
		mWorkerThreadCount = c;
	}
	//---------------------------------------------------------------------
	UINT16 WorkQueue::getChannel(const String& channelName)
	{
		CM_LOCK_MUTEX(mChannelMapMutex)

		ChannelMap::iterator i = mChannelMap.find(channelName);
		if (i == mChannelMap.end())
		{
			i = mChannelMap.insert(ChannelMap::value_type(channelName, mNextChannel++)).first;
		}
		return i->second;
	}
	//---------------------------------------------------------------------
	void WorkQueue::processRequestResponse(Request* r)
	{
		Response* response = processRequest(r);

		CM_LOCK_MUTEX(mProcessMutex)

		RequestQueue::iterator it;
		for( it = mProcessQueue.begin(); it != mProcessQueue.end(); ++it )
		{
			if( (*it) == r )
			{
				mProcessQueue.erase( it );
				break;
			}
		}

		if (response)
		{
			if (!response->succeeded())
			{
				// Failed, should we retry?
				const Request* req = response->getRequest();
				if (req->getRetryCount())
				{
					addRequestWithRID(req->getID(), req->getChannel(), req->getData(), 
						req->getRetryCount() - 1);
					// discard response (this also deletes request)
					CM_DELETE(response, Response, ScratchAlloc);
					return;
				}
			}

			processResponse(response);
			CM_DELETE(response, Response, ScratchAlloc);
		}
		else
		{
			// no response, delete request
			gDebug().logWarning("warning: no handler processed request "
				+ toString(r->getID()) + ", channel " + toString(r->getChannel()));

			CM_DELETE(r, Request, ScratchAlloc);
		}

	}
	WorkQueue::Response* WorkQueue::processRequest(Request* r)
	{
		RequestHandlerListByChannel handlerListCopy;
		{
			// lock the list only to make a copy of it, to maximise parallelism
			CM_LOCK_RW_MUTEX_READ(mRequestHandlerMutex);

			handlerListCopy = mRequestHandlers;

		}

		Response* response = 0;

		RequestHandlerListByChannel::iterator i = handlerListCopy.find(r->getChannel());
		if (i != handlerListCopy.end())
		{
			RequestHandlerList& handlers = i->second;
			for (RequestHandlerList::reverse_iterator j = handlers.rbegin(); j != handlers.rend(); ++j)
			{
				// threadsafe call which tests canHandleRequest and calls it if so 
				response = (*j)->handleRequest(r, this);

				if (response)
					break;
			}
		}

		return response;

	}
	//---------------------------------------------------------------------
	void WorkQueue::processResponse(Response* r)
	{
		ResponseHandlerListByChannel::iterator i = mResponseHandlers.find(r->getRequest()->getChannel());
		if (i != mResponseHandlers.end())
		{
			ResponseHandlerList& handlers = i->second;
			for (ResponseHandlerList::reverse_iterator j = handlers.rbegin(); j != handlers.rend(); ++j)
			{
				if ((*j)->canHandleResponse(r, this))
				{
					(*j)->handleResponse(r, this);
				}
			}
		}
	}
	//---------------------------------------------------------------------
	void WorkQueue::processNextRequest()
	{
		Request* request = 0;
		{
			// scoped to only lock while retrieving the next request
			CM_LOCK_MUTEX(mProcessMutex)
			{
				CM_LOCK_MUTEX(mRequestMutex)

					if (!mRequestQueue.empty())
					{
						request = mRequestQueue.front();
						mRequestQueue.pop_front();
						mProcessQueue.push_back( request );
					}
			}
		}

		if (request)
		{
			processRequestResponse(request);
		}


	}
	//---------------------------------------------------------------------
	void WorkQueue::waitForNextRequest()
	{
#if CM_THREAD_SUPPORT
		// Lock; note that OGRE_THREAD_WAIT will free the lock
		CM_LOCK_MUTEX_NAMED(mRequestMutex, queueLock);
		if (mRequestQueue.empty())
		{
			// frees lock and suspends the thread
			CM_THREAD_WAIT(mRequestCondition, mRequestMutex, queueLock);
		}
		// When we get back here, it's because we've been notified 
		// and thus the thread has been woken up. Lock has also been
		// re-acquired, but we won't use it. It's safe to try processing and fail
		// if another thread has got in first and grabbed the request
#endif

	}
	//---------------------------------------------------------------------
	void WorkQueue::threadMain()
	{
		// default worker thread
#if CM_THREAD_SUPPORT
		// Spin forever until we're told to shut down
		while (!isShuttingDown())
		{
			waitForNextRequest();
			processNextRequest();
		}
#endif
	}
	//---------------------------------------------------------------------
	void WorkQueue::notifyWorkers()
	{
		// wake up waiting thread
		CM_THREAD_NOTIFY_ONE(mRequestCondition)
	}
	//---------------------------------------------------------------------
	WorkQueue::Request::Request(UINT16 channel, const boost::any& rData, UINT8 retry, RequestID rid)
		: mChannel(channel), mData(rData), mRetryCount(retry), mID(rid), mAborted(false)
	{

	}
	//---------------------------------------------------------------------
	WorkQueue::Request::~Request()
	{

	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	WorkQueue::Response::Response(Request* rq, bool success, const boost::any& data)
		: mRequest(rq), mSuccess(success), mData(data)
	{

	}
	//---------------------------------------------------------------------
	WorkQueue::Response::~Response()
	{
		CM_DELETE(mRequest, Request, ScratchAlloc);
	}
	//---------------------------------------------------------------------
	void WorkQueue::WorkerFunc::operator()()
	{
		mQueue->threadMain();
	}

	void WorkQueue::WorkerFunc::run()
	{
		mQueue->threadMain();
	}
}
