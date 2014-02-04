#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObject.h"
#include "CmSceneObject.h"
#include "CmGameObjectManager.h"

namespace CamelotFramework
{
	class CM_EXPORT GameObjectRTTI : public RTTIType<GameObject, IReflectable, GameObjectRTTI>
	{
	private:
		UINT64& getInstanceID(GameObject* obj) { return obj->mInstanceData->mInstanceId; }
		void setInstanceID(GameObject* obj, UINT64& instanceId) 
		{  
			// The system will have already assigned the instance ID, but since other objects might be referencing
			// the old (serialized) ID we store it in the GameObjectSerializationManager so we can map from old to new id.
			GameObjectManager::instance().registerDeserializedId(instanceId, obj->getInstanceId());
		}

	public:
		template <typename T>
		static std::shared_ptr<T> createGameObject()
		{
			return SceneObject::createEmptyComponent<T>();
		}

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