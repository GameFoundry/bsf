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

	GameObjectHandleBase GameObjectManager::getObject(UINT64 id) const 
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

	UINT64 GameObjectManager::registerObject(const GameObjectHandleBase& object)
	{
		mObjects[mNextAvailableID] = object;

		return mNextAvailableID++;
	}

	void GameObjectManager::unregisterObject(const GameObjectHandleBase& object)
	{
		mObjects.erase(object->getInstanceID());
	}
}