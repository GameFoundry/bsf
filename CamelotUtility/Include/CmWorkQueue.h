#pragma once

#include "CmPrerequisitesUtil.h"
#include "boost/any.hpp"

namespace CamelotFramework
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/

	/** Interface to a general purpose request / response style background work queue.
	@remarks
		A work queue is a simple structure, where requests for work are placed
		onto the queue, then removed by a worker for processing, then finally
		a response is placed on the result queue for the originator to pick up
		at their leisure. The typical use for this is in a threaded environment, 
		although any kind of deferred processing could use this approach to 
		decouple and distribute work over a period of time even 
		if it was single threaded.
	@par
		WorkQueues also incorporate thread pools. One or more background worker threads
		can wait on the queue and be notified when a request is waiting to be
		processed. For maximal thread usage, a WorkQueue instance should be shared
		among many sources of work, rather than many work queues being created.
		This way, you can share a small number of hardware threads among a large 
		number of background tasks. This doesn't mean you have to implement all the
		request processing in one class, you can plug in many handlers in order to
		process the requests.
	*/
	class CM_UTILITY_EXPORT WorkQueue
	{
	protected:
		typedef Map<String, UINT16>::type ChannelMap;
		ChannelMap mChannelMap;
		UINT16 mNextChannel;
		CM_MUTEX(mChannelMapMutex)
	public:
		/// Numeric identifier for a request
		typedef unsigned long long int RequestID;

		/** General purpose request structure. 
		*/
		class CM_UTILITY_EXPORT Request
		{
			friend class WorkQueue;
		protected:
			/// The request channel, as an integer 
			UINT16 mChannel;
			/// The details of the request (user defined)
			boost::any mData;
			/// Retry count - set this to non-zero to have the request try again on failure
			UINT8 mRetryCount;
			/// Identifier (assigned by the system)
			RequestID mID;
			/// Abort Flag
			mutable bool mAborted;

		public:
			/// Constructor 
			Request(UINT16 channel, const boost::any& rData, UINT8 retry, RequestID rid);
			~Request();
			/// Set the abort flag
			void abortRequest() const { mAborted = true; }
			/// Get the request channel (top level categorisation)
			UINT16 getChannel() const { return mChannel; }
			/// Get the user details of this request
			const boost::any& getData() const { return mData; }
			/// Get the remaining retry count
			UINT8 getRetryCount() const { return mRetryCount; }
			/// Get the identifier of this request
			RequestID getID() const { return mID; }
			/// Get the abort flag
			bool getAborted() const { return mAborted; }
		};

		/** General purpose response structure. 
		*/
		struct CM_UTILITY_EXPORT Response
		{
			/// Pointer to the request that this response is in relation to
			Request* mRequest;
			/// Whether the work item succeeded or not
			bool mSuccess;
			/// Data associated with the result of the process
			boost::any mData;

		public:
			Response(Request* rq, bool success, const boost::any& data);
			~Response();
			/// Get the request that this is a response to (NB destruction destroys this)
			const Request* getRequest() const { return mRequest; }
			/// Return whether this is a successful response
			bool succeeded() const { return mSuccess; }
			/// Return the response data (user defined, only valid on success)
			const boost::any& getData() const { return mData; }
			/// Abort the request
			void abortRequest() { mRequest->abortRequest(); }
		};

		/** Interface definition for a handler of requests. 
		@remarks
		User classes are expected to implement this interface in order to
		process requests on the queue. It's important to realise that
		the calls to this class may be in a separate thread to the main
		render context, and as such it may not be possible to make
		rendersystem or other GPU-dependent calls in this handler. You can only
		do so if the queue was created with 'workersCanAccessRenderSystem'
		set to true, and OGRE_THREAD_SUPPORT=1, but this puts extra strain
		on the thread safety of the render system and is not recommended.
		It is best to perform CPU-side work in these handlers and let the
		response handler transfer results to the GPU in the main render thread.
		*/
		class CM_UTILITY_EXPORT RequestHandler
		{
		public:
			RequestHandler() {}
			virtual ~RequestHandler() {}

			/** Return whether this handler can process a given request. 
			@remarks
			Defaults to true, but if you wish to add several handlers each of
			which deal with different types of request, you can override
			this method. 
			*/
			virtual bool canHandleRequest(const Request* req, const WorkQueue* srcQ) 
			{ (void)srcQ; return !req->getAborted(); }

			/** The handler method every subclass must implement. 
			If a failure is encountered, return a Response with a failure
			result rather than raise an exception.
			@param req The Request structure, which is effectively owned by the
			handler during this call. It must be attached to the returned
			Response regardless of success or failure.
			@param srcQ The work queue that this request originated from
			@return Pointer to a Response object - the caller is responsible
			for deleting the object.
			*/
			virtual Response* handleRequest(Request* req, const WorkQueue* srcQ) = 0;
		};

		/** Interface definition for a handler of responses. 
		@remarks
		User classes are expected to implement this interface in order to
		process responses from the queue. All calls to this class will be 
		in the main render thread and thus all GPU resources will be
		available. 
		*/
		class CM_UTILITY_EXPORT ResponseHandler
		{
		public:
			ResponseHandler() {}
			virtual ~ResponseHandler() {}

			/** Return whether this handler can process a given response. 
			@remarks
			Defaults to true, but if you wish to add several handlers each of
			which deal with different types of response, you can override
			this method. 
			*/
			virtual bool canHandleResponse(const Response* res, const WorkQueue* srcQ) 
			{ (void)srcQ; return !res->getRequest()->getAborted(); }

			/** The handler method every subclass must implement. 
			@param res The Response structure. The caller is responsible for
			deleting this after the call is made, none of the data contained
			(except pointers to structures in user Any data) will persist
			after this call is returned.
			@param srcQ The work queue that this request originated from
			*/
			virtual void handleResponse(const Response* res, const WorkQueue* srcQ) = 0;
		};

	protected:
		size_t mWorkerThreadCount;
		bool mIsRunning;
		unsigned long mResposeTimeLimitMS;

		typedef Deque<Request*>::type RequestQueue;
		RequestQueue mRequestQueue;
		RequestQueue mProcessQueue;

		/// Thread function
		struct WorkerFunc CM_THREAD_WORKER_INHERIT
		{
			WorkQueue* mQueue;

			WorkerFunc(WorkQueue* q) 
				: mQueue(q) {}

			void operator()();

			void run();
		};
		WorkerFunc* mWorkerFunc;

		/** Intermediate structure to hold a pointer to a request handler which 
			provides insurance against the handler itself being disconnected
			while the list remains unchanged.
		*/
		class CM_UTILITY_EXPORT RequestHandlerHolder
		{
		protected:
			CM_RW_MUTEX(mRWMutex);
			RequestHandler* mHandler;
		public:
			RequestHandlerHolder(RequestHandler* handler)
				: mHandler(handler)	{}

			// Disconnect the handler to allow it to be destroyed
			void disconnectHandler()
			{
				// write lock - must wait for all requests to finish
				CM_LOCK_RW_MUTEX_WRITE(mRWMutex);
				mHandler = 0;
			}

			/** Get handler pointer - note, only use this for == comparison or similar,
				do not attempt to call it as it is not thread safe. 
			*/
			RequestHandler* getHandler() { return mHandler; }

			/** Process a request if possible.
			@return Valid response if processed, null otherwise
			*/
			Response* handleRequest(Request* req, const WorkQueue* srcQ)
			{
				// Read mutex so that multiple requests can be processed by the
				// same handler in parallel if required
				CM_LOCK_RW_MUTEX_READ(mRWMutex);
				Response* response = 0;
				if (mHandler)
				{
					if (mHandler->canHandleRequest(req, srcQ))
					{
						response = mHandler->handleRequest(req, srcQ);
					}
				}
				return response;
			}

		};
		// Hold these by shared pointer so they can be copied keeping same instance
		typedef std::shared_ptr<RequestHandlerHolder> RequestHandlerHolderPtr;

		typedef List<RequestHandlerHolderPtr>::type RequestHandlerList;
		typedef List<ResponseHandler*>::type ResponseHandlerList;
		typedef Map<UINT16, RequestHandlerList>::type RequestHandlerListByChannel;
		typedef Map<UINT16, ResponseHandlerList>::type ResponseHandlerListByChannel;

		RequestHandlerListByChannel mRequestHandlers;
		ResponseHandlerListByChannel mResponseHandlers;
		RequestID mRequestCount;
		bool mPaused;
		bool mAcceptRequests;
		bool mShuttingDown;

		/// Synchroniser token to wait / notify on thread init 
		CM_THREAD_SYNCHRONISER(mInitSync)

		CM_THREAD_SYNCHRONISER(mRequestCondition)

		/// Init notification mutex (must lock before waiting on initCondition)
		CM_MUTEX(mRequestMutex)
		CM_MUTEX(mProcessMutex)
		CM_RW_MUTEX(mRequestHandlerMutex);

#if CM_THREAD_SUPPORT
		typedef Vector<CM_THREAD_TYPE*>::type WorkerThreadList;
		WorkerThreadList mWorkers;
#endif

	public:
		WorkQueue();
		~WorkQueue();		

		/** Start up the queue with the options that have been set.
		@param forceRestart If the queue is already running, whether to shut it
			down and restart.
		*/
		void startup(bool forceRestart = true);

		/** Shut down the queue.
		*/
		void shutdown();

		/** Add a request handler instance to the queue. 
		@remarks
			Every queue must have at least one request handler instance for each 
			channel in which requests are raised. If you 
			add more than one handler per channel, then you must implement canHandleRequest 
			differently	in each if you wish them to respond to different requests.
		@param channel The channel for requests you want to handle
		@param rh Your handler
		*/
		void addRequestHandler(UINT16 channel, RequestHandler* rh);
		/** Remove a request handler. */
		void removeRequestHandler(UINT16 channel, RequestHandler* rh);

		/** Add a response handler instance to the queue. 
		@remarks
			Every queue must have at least one response handler instance for each 
			channel in which requests are raised. If you add more than one, then you 
			must implement canHandleResponse differently in each if you wish them 
			to respond to different responses.
		@param channel The channel for responses you want to handle
		@param rh Your handler
		*/
		void addResponseHandler(UINT16 channel, ResponseHandler* rh);
		/** Remove a Response handler. */
		void removeResponseHandler(UINT16 channel, ResponseHandler* rh);

		/**
		 * @brief	Gets the next free request identifier.
		 *
		 * @return	The next free request identifier.
		 */
		RequestID peekNextFreeRequestId();

		/** Add a new request to the queue.
		@param channel The channel this request will go into = 0; the channel is the top-level
			categorisation of the request
		@param requestType An identifier that's unique within this queue which
			identifies the type of the request (user decides the actual value)
		@param rData The data required by the request process. 
		@param retryCount The number of times the request should be retried
			if it fails.
		@param forceSynchronous Forces the request to be processed immediately
			even if threading is enabled.
		@returns The ID of the request that has been added
		*/
		RequestID addRequest(UINT16 channel, const boost::any& rData, UINT8 retryCount = 0, 
			bool forceSynchronous = false);

		/// Put a Request on the queue with a specific RequestID.
		void addRequestWithRID(RequestID rid, UINT16 channel, const boost::any& rData, UINT8 retryCount);

		/** Abort a previously issued request.
		If the request is still waiting to be processed, it will be 
		removed from the queue.
		@param id The ID of the previously issued request.
		*/
		void abortRequest(RequestID id);

		/** Abort all previously issued requests in a given channel.
		Any requests still waiting to be processed of the given channel, will be 
		removed from the queue.
		@param channel The type of request to be aborted
		*/
		void abortRequestsByChannel(UINT16 channel);

		/** Abort all previously issued requests.
		Any requests still waiting to be processed will be removed from the queue.
		Any requests that are being processed will still complete.
		*/
		void abortAllRequests();
		
		/** Set whether to pause further processing of any requests. 
		If true, any further requests will simply be queued and not processed until
		setPaused(false) is called. Any requests which are in the process of being
		worked on already will still continue. 
		*/
		void setPaused(bool pause);
		/// Return whether the queue is paused ie not sending more work to workers
		bool isPaused() const;

		/** Set whether to accept new requests or not. 
		If true, requests are added to the queue as usual. If false, requests
		are silently ignored until setRequestsAccepted(true) is called. 
		*/
		void setRequestsAccepted(bool accept);
		/// Returns whether requests are being accepted right now
		bool getRequestsAccepted() const;

		/** Get the number of worker threads that this queue will start when 
			startup() is called. 
		*/
		size_t getWorkerThreadCount() const;

		/** Set the number of worker threads that this queue will start
			when startup() is called (default 1).
			Calling this will have no effect unless the queue is shut down and
			restarted.
		*/
		void setWorkerThreadCount(size_t c);

		/** Get a channel ID for a given channel name. 
		@remarks
			Channels are assigned on a first-come, first-served basis and are
			not persistent across application instances. This method allows 
			applications to not worry about channel clashes through manually
			assigned channel numbers.
		*/
		UINT16 getChannel(const String& channelName);

		/** Returns whether the queue is trying to shut down. */
		bool isShuttingDown() const { return mShuttingDown; }

	protected:
		void processRequestResponse(Request* r);
		Response* processRequest(Request* r);
		void processResponse(Response* r);

		/** To be called by a separate thread; will return immediately if there
			are items in the queue, or suspend the thread until new items are added
			otherwise.
		*/
		void waitForNextRequest();

		/** Process the next request on the queue. 
		@remarks
			This method is public, but only intended for advanced users to call. 
			The only reason you would call this, is if you were using your 
			own thread to drive the worker processing. The thread calling this
			method will be the thread used to call the RequestHandler.
		*/
		void processNextRequest();

		/// Main function for each thread spawned.
		void threadMain();

		/// Notify workers about a new request. 
		void notifyWorkers();
	};
}