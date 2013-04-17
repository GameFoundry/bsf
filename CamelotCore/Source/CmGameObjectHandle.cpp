#include "CmPrerequisites.h"
#include "CmGameObject.h"
#include "CmGameObjectHandle.h"
#include "CmException.h"

namespace CamelotFramework
{
	GameObjectHandleBase::GameObjectHandleBase()
	{ }

	void GameObjectHandleBase::throwIfDestroyed() const
	{
		if(isDestroyed()) 
		{
			CM_EXCEPT(InternalErrorException, "Trying to access an object that has been destroyed.");
		}
	}
}