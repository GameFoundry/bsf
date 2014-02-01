#include "CmGameObject.h"
#include "CmGameObjectRTTI.h"
#include "CmGameObjectManager.h"

namespace CamelotFramework
{
	GameObject::GameObject()
		:mInstanceId(0)
	{
		mInstanceId = GameObjectManager::instance().registerObject(this);
	}

	GameObject::~GameObject()
	{
		GameObjectManager::instance().unregisterObject(this);
	}
	
	RTTITypeBase* GameObject::getRTTIStatic()
	{
		return GameObjectRTTI::instance();
	}

	RTTITypeBase* GameObject::getRTTI() const
	{
		return GameObject::getRTTIStatic();
	}
}