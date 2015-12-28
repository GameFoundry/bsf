#include "BsAsyncOp.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	bool AsyncOp::hasCompleted() const 
	{ 
		return mData->mIsCompleted.load(std::memory_order_acquire);
	}

	void AsyncOp::_completeOperation(Any returnValue) 
	{ 
		mData->mReturnValue = returnValue; 
		mData->mIsCompleted.store(true, std::memory_order_release);

		if (mSyncData != nullptr)
			BS_THREAD_NOTIFY_ALL(mSyncData->mCondition);
	}

	void AsyncOp::_completeOperation() 
	{ 
		mData->mIsCompleted.store(true, std::memory_order_release);

		if (mSyncData != nullptr)
			BS_THREAD_NOTIFY_ALL(mSyncData->mCondition);
	}

	void AsyncOp::blockUntilComplete() const
	{
		if (mSyncData == nullptr)
		{
			LOGERR("No sync data is available. Cannot block until AsyncOp is complete.");
			return;
		}

		BS_LOCK_MUTEX_NAMED(mSyncData->mMutex, lock);
		while (!hasCompleted())
			BS_THREAD_WAIT(mSyncData->mCondition, mSyncData->mMutex, lock);
	}
}