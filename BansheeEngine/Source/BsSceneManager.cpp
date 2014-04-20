#include "BsSceneManager.h"
#include "CmSceneManager.h"

namespace BansheeEngine
{
	SceneManager& gBsSceneManager()
	{
		return static_cast<SceneManager&>(gSceneManager());
	}
}
