#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"
#include "boost/any.hpp"

namespace CamelotFramework
{
	/**
	 * @brief	Asynchronous operation. Contains uninitialized data until
	 * 			isResolved returns true.
	 * 			
	 * @note	You are allowed (and meant to) to copy this by value.
	 */
	class AsyncOp
	{
	private:
		struct AsyncOpData
		{
			AsyncOpData()
				:mIsCompleted(false)
			{ }

			boost::any mReturnValue;
			bool mIsCompleted;
		};

	public:
		AsyncOp()
			:mData(CM_NEW(AsyncOpData, ScratchAlloc) AsyncOpData(), &MemAllocDeleter<AsyncOpData, ScratchAlloc>::deleter)
		{}

		/**
		 * @brief	True if the async operation has completed.
		 */
		bool hasCompleted() const { return mData->mIsCompleted; }

		/**
		 * @brief	Mark the async operation as completed.
		 */
		void completeOperation(boost::any returnValue) { mData->mReturnValue = returnValue; mData->mIsCompleted = true; }

		/**
		 * @brief	Mark the async operation as completed, without setting a return value;
		 */
		void completeOperation() { mData->mIsCompleted = true; }

		/**
		 * @brief	Retrieves the value returned by the async operation.
		 */
		template <typename T>
		T getReturnValue() const 
		{ 
#if CM_DEBUG_MODE
			if(!hasCompleted())
				CM_EXCEPT(InternalErrorException, "Trying to get AsyncOp return value but the operation hasn't completed.");
#endif
			// Be careful if boost returns bad_any_cast. It doesn't support casting of polymorphic types. Provided and returned
			// types must be EXACT. (You'll have to cast the data yourself when completing the operation)
			return boost::any_cast<T>(mData->mReturnValue);
		}

	private:
		std::shared_ptr<AsyncOpData> mData;
	};
}