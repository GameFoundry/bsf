#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmException.h"
#include "BsAny.h"

namespace BansheeEngine
{
	/**
	 * @brief	Object you may use to check on the results of an asynchronous operation. 
	 *			Contains uninitialized data until "hasCompleted" returns true. 
	 * 			
	 * @note	You are allowed (and meant to) to copy this by value.
	 * 			
	 *			You'll notice mIsCompleted isn't locked. This is safe on x86 architectures because all stores
	 *			are executed in order. Loads may be executed out of order from stores but worst case scenario is that
	 *			mIsCompleted reports false a few cycles too late, which is not relevant for practical use.
	 */
	class BS_UTILITY_EXPORT AsyncOp
	{
	private:
		struct AsyncOpData
		{
			AsyncOpData()
				:mIsCompleted(false)
			{ }

			Any mReturnValue;
			volatile bool mIsCompleted;
		};

	public:
		AsyncOp()
			:mData(cm_shared_ptr<AsyncOpData, ScratchAlloc>())
		{
#if CM_ARCH_TYPE != CM_ARCHITECTURE_x86_32 && CM_ARCH_TYPE != CM_ARCHITECTURE_x86_64
			static_assert(false, "You will likely need to add locks for mIsCompleted on architectures other than x86.");
#endif
		}

		/**
		 * @brief	True if the async operation has completed.
		 */
		bool hasCompleted() const { return mData->mIsCompleted; }

		/**
		 * @brief	Internal method. Mark the async operation as completed.
		 */
		void _completeOperation(Any returnValue);

		/**
		 * @brief	Internal method. Mark the async operation as completed, without setting a return value.
		 */
		void _completeOperation();

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
			// Be careful if cast throws an exception. It doesn't support casting of polymorphic types. Provided and returned
			// types must be EXACT. (You'll have to cast the data yourself when completing the operation)
			return any_cast<T>(mData->mReturnValue);
		}

	private:
		std::shared_ptr<AsyncOpData> mData;
	};
}