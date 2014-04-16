#include "CmAsyncOp.h"

namespace CamelotFramework
{
	void AsyncOp::_completeOperation(boost::any returnValue) 
	{ 
		mData->mReturnValue = returnValue; 
		mData->mIsCompleted = true; 
	}

	void AsyncOp::_completeOperation() 
	{ 
		mData->mIsCompleted = true; 
	}
}