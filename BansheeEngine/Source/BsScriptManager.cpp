#include "BsScriptManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void ScriptManager::initialize(const std::shared_ptr<ScriptSystem>& scriptSystem)
	{
		mScriptSystem = scriptSystem;
		mScriptSystem->initialize();
	}

	void ScriptManager::onShutDown()
	{
		mScriptSystem = nullptr;
	}
}