#include "CmGameObject.h"
#include "CmGameObjectRTTI.h"
#include "CmGameObjectManager.h"

namespace CamelotFramework
{
	GameObject::GameObject()
		:mInstanceId(0)
	{ }

	GameObject::~GameObject()
	{ }
	
	RTTITypeBase* GameObject::getRTTIStatic()
	{
		return GameObjectRTTI::instance();
	}

	RTTITypeBase* GameObject::getRTTI() const
	{
		return GameObject::getRTTIStatic();
	}
}