#include "BsThreadPolicy.h"

namespace BansheeEngine
{
	void ThreadBansheePolicy::onThreadStarted(const String& name) 
	{
		MemStack::beginThread();
	}

	void ThreadBansheePolicy::onThreadEnded(const String& name) 
	{
		MemStack::endThread();
	}
}