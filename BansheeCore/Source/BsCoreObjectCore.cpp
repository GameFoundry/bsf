#include "BsCoreObjectCore.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	CoreObjectCore::CoreObjectCore()
		:mCoreDirtyFlags(0xFFFFFFFF)
	{ }

	CoreObjectCore::~CoreObjectCore()
	{ 
		THROW_IF_NOT_CORE_THREAD;
	}

	void CoreObjectCore::_setThisPtr(std::shared_ptr<CoreObjectCore> ptrThis)
	{
		mThis = ptrThis;
	}
}