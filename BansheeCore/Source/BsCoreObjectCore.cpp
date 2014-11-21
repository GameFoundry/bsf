#include "BsCoreObjectCore.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	CoreObjectCore::CoreObjectCore()
		:mCoreDirtyFlags(0xFFFFFFFF), mIsDestroyed(false)
	{ }

	CoreObjectCore::~CoreObjectCore()
	{
		// This should only trigger for objects created directly by core thread.
		// If you are not sure this will get called by the core thread, make sure 
		// to destroy() the object manually before it goes out of scope.
		if (!mIsDestroyed)
			destroy(); //assert(false && "Destructing an object that has not been destroyed previously. This is not allowed.");
	}

	void CoreObjectCore::destroy() 
	{ 
		throwIfNotCoreThread();

		mIsDestroyed = true;
	}

	void CoreObjectCore::_setThisPtr(std::shared_ptr<CoreObjectCore> ptrThis)
	{
		mThis = ptrThis;
	}
}