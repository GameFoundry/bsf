//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAsyncOpEx.h"
#include "BsCoreThread.h"

namespace bs
{
	AsyncOpEx::AsyncOpEx(const AsyncOp& op, const std::function<ScriptObjectBase*(const AsyncOp&)>& convertCallback)
		:mAsyncOp(op), mConvertCallback(convertCallback)
	{ }

	bool AsyncOpEx::isComplete() const
	{
		return mAsyncOp.hasCompleted();
	}

	ScriptObjectBase* AsyncOpEx::getReturnValue() const
	{
		if (!mAsyncOp.hasCompleted())
			return nullptr;

		if (mConvertCallback == nullptr)
			return nullptr;

		return mConvertCallback(mAsyncOp);
	}

	void AsyncOpEx::blockUntilComplete() const
	{
		if (!mAsyncOp.hasCompleted())
		{
			gCoreThread().submit();
			mAsyncOp.blockUntilComplete();
		}
	}
}