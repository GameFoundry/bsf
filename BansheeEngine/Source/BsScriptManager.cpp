#include "BsScriptManager.h"

namespace BansheeEngine
{
	void ScriptManager::initialize()
	{
		if (mScriptLibrary != nullptr)
			mScriptLibrary->initialize();
	}

	void ScriptManager::reload()
	{
		if (mScriptLibrary != nullptr)
			mScriptLibrary->reload();
	}

	void ScriptManager::onShutDown()
	{
		if (mScriptLibrary != nullptr)
			mScriptLibrary->destroy();
	}

	void ScriptManager::_setScriptLibrary(const SPtr<ScriptLibrary>& library)
	{
		mScriptLibrary = library;
	}
}