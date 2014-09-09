#include "BsSceneManager.h"

namespace BansheeEngine
{
	SceneManager& SceneManager::instance()
	{
		return static_cast<SceneManager&>(CoreSceneManager::instance());
	}

	SceneManager* SceneManager::instancePtr()
	{
		return static_cast<SceneManager*>(CoreSceneManager::instancePtr());
	}

	SceneManager& gSceneManager()
	{
		return static_cast<SceneManager&>(gCoreSceneManager());
	}
}
