#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObject.h"
#include "BsSceneObject.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GameObjectRTTI : public RTTIType<GameObject, IReflectable, GameObjectRTTI>
	{
	private:
		String& getName(GameObject* obj) { return obj->mName; }
		void setName(GameObject* obj, String& name) { obj->mName = name; }

		UINT64& getInstanceID(GameObject* obj) { return obj->mInstanceData->mInstanceId; }
		void setInstanceID(GameObject* obj, UINT64& instanceId) 
		{  
			// The system will have already assigned the instance ID, but since other objects might be referencing
			// the old (serialized) ID we store it in the GameObjectSerializationManager so we can map from old to new id.
			GameObjectManager::instance().registerDeserializedId(instanceId, obj->getInstanceId());
		}

		INT32& getLinkId(GameObject* obj) { return obj->mLinkId; }
		void setLinkId(GameObject* obj, INT32& linkId) { obj->mLinkId = linkId; }

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
			addPlainField("mName", 1, &GameObjectRTTI::getName, &GameObjectRTTI::setName);
			addPlainField("mLinkId", 2, &GameObjectRTTI::getLinkId, &GameObjectRTTI::setLinkId);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "GameObject";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_GameObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}