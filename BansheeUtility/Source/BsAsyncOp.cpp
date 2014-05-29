#include "BsAsyncOp.h"

namespace BansheeEngine
{
	void AsyncOp::_completeOperation(Any returnValue) 
	{ 
		mData->mReturnValue = returnValue; 
		mData->mIsCompleted = true; 
	}

	void AsyncOp::_completeOperation() 
	{ 
		mData->mIsCompleted = true; 
	}
}