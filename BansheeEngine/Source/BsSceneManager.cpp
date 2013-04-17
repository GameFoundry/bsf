#include "BsSceneManager.h"
#include "CmSceneManager.h"

namespace BansheeEngine
{
	SceneManager& gSceneManager()
	{
		return static_cast<BS::SceneManager&>(CM::gSceneManager());
	}
}
