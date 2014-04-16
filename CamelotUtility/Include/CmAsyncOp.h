#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"
#include "boost/any.hpp"

namespace CamelotFramework
{
	/**
	 * @brief	Object you may use to check on the results of an asynchronous operation. 
	 *			Contains uninitialized data until "hasCompleted" returns true. 
	 * 			
	 * @note	You are allowed (and meant to) to copy this by value.
	 */
	class CM_UTILITY_EXPORT AsyncOp
	{
	private:
		struct AsyncOpData
		{
			AsyncOpData()
				:mIsCompleted(false)
			{ }

			boost::any mReturnValue;
			volatile bool mIsCompleted;
		};

	public:
		AsyncOp()
			:mData(cm_shared_ptr<AsyncOpData, ScratchAlloc>())
		{}

		/**
		 * @brief	True if the async operation has completed.
		 */
		bool hasCompleted() const { return mData->mIsCompleted; }

		/**
		 * @brief	Internal method. Mark the async operation as completed.
		 */
		void completeOperation(boost::any returnValue);

		/**
		 * @brief	Internal method. Mark the async operation as completed, without setting a return value.
		 */
		void completeOperation();

		/**
		 * @brief	Retrieves the value returned by the async operation. Only valid
		 *			if "hasCompleted" returns true.
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