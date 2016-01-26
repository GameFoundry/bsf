#include "BsSceneManager.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	SceneManager& gBsSceneManager()
	{
		return static_cast<SceneManager&>(gSceneManager());
	}
}
