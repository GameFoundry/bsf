#include "BsMonoScriptManager.h"
#include "BsMonoManager.h"

using namespace CamelotFramework;

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
		MonoManager::startUp();
		mIsInitialized = true;
	}
}