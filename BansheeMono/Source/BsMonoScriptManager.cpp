#include "BsMonoScriptManager.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	MonoScriptSystem::MonoScriptSystem()
		:mIsInitialized(false)
	{

	}

	MonoScriptSystem::~MonoScriptSystem()
	{
		if(mIsInitialized)
			MonoManager::shutDown();
	}

	void MonoScriptSystem::initialize()
	{
		MonoManager::startUp(cm_new<MonoManager>());
		mIsInitialized = true;
	}

	void MonoScriptSystem::destroy()
	{
		if (mIsInitialized)
		{
			MonoManager::shutDown();
			mIsInitialized = false;
		}
	}
}