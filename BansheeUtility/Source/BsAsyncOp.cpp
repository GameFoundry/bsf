//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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