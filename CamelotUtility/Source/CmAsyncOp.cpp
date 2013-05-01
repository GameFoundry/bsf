#include "CmAsyncOp.h"

namespace CamelotFramework
{
	void AsyncOp::completeOperation(boost::any returnValue) 
	{ 
		mData->mReturnValue = returnValue; 
		mData->mIsCompleted = true; 
	}

	void AsyncOp::completeOperation() 
	{ 
		mData->mIsCompleted = true; 
	}
}