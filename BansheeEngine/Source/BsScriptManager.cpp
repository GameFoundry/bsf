#include "BsScriptManager.h"

namespace BansheeEngine
{
	void ScriptManager::initialize(const std::shared_ptr<ScriptSystem>& scriptSystem)
	{
		mScriptSystem = scriptSystem;
		mScriptSystem->initialize();
	}

	void ScriptManager::destroy()
	{
		if (mScriptSystem != nullptr)
			mScriptSystem->destroy();
	}

	void ScriptManager::onShutDown()
	{
		mScriptSystem = nullptr;
	}
}