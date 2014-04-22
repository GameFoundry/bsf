#include "BsThreadPolicy.h"

namespace BansheeEngine
{
	void ThreadBansheePolicy::onThreadStarted(const String& name) 
	{
		MemStack::beginThread();
		Profiler::instance().beginThread(name.c_str());
	}

	void ThreadBansheePolicy::onThreadEnded(const String& name) 
	{
		Profiler::instance().endThread();
		MemStack::endThread();
	}
}