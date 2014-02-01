#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	class CM_EXPORT GameObjectRTTI : public RTTIType<GameObject, IReflectable, GameObjectRTTI>
	{
	private:
		UINT64& getInstanceID(GameObject* obj) { return obj->mInstanceId; }
		void setInstanceID(GameObject* obj, UINT64& instanceId) { obj->mInstanceId = instanceId; }

	public:
		GameObjectRTTI()
		{
			addPlainField("mInstanceID", 0, &GameObjectRTTI::getInstanceID, &GameObjectRTTI::setInstanceID);
		}

		virtual const String& getRTTIName()
		{
			static String name = "GameObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GameObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}