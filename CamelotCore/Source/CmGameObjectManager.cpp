#include "CmGameObjectManager.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	GameObjectManager::GameObjectManager()
		:mNextAvailableID(1)
	{

	}

	GameObjectManager::~GameObjectManager()
	{ }

	GameObject* GameObjectManager::getObject(UINT64 id) const 
	{ 
		auto iterFind = mObjects.find(id);

		if(iterFind != mObjects.end())
			return iterFind->second;
		
		return nullptr;
	}

	bool GameObjectManager::objectExists(UINT64 id) const 
	{ 
		return mObjects.find(id) != mObjects.end(); 
	}

	UINT64 GameObjectManager::registerObject(GameObject* object)
	{
		assert(object != nullptr);

		mObjects[mNextAvailableID] = object;

		return mNextAvailableID++;
	}

	void GameObjectManager::unregisterObject(GameObject* object)
	{
		assert(object != nullptr);

		mObjects.erase(object->getInstanceID());
	}
}