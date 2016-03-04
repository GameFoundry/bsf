//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObject.h"
#include "BsSceneObject.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	/**	Provides temporary storage for data used during GameObject deserialization. */
	struct GODeserializationData
	{
		GODeserializationData()
			:isDeserializationParent(false), originalId(0)
		{ }

		GameObjectPtr ptr;
		bool isDeserializationParent;
		UINT64 originalId;
		Any moreData;
	};

	class BS_CORE_EXPORT GameObjectRTTI : public RTTIType<GameObject, IReflectable, GameObjectRTTI>
	{
	private:
		String& getName(GameObject* obj) { return obj->mName; }
		void setName(GameObject* obj, String& name) { obj->mName = name; }

		UINT64& getInstanceID(GameObject* obj) { return obj->mInstanceData->mInstanceId; }
		void setInstanceID(GameObject* obj, UINT64& instanceId) 
		{  
			// We record the ID for later use. Any child RTTI of GameObject must call GameObjectManager::registerObject
			// with this ID, so we know how to map deserialized GO handles to live objects, otherwise the handle
			// references will get broken.
			GameObject* go = static_cast<GameObject*>(obj);
			GODeserializationData& deserializationData = any_cast_ref<GODeserializationData>(go->mRTTIData);

			deserializationData.originalId = instanceId;
		}

		UINT32& getLinkId(GameObject* obj) { return obj->mLinkId; }
		void setLinkId(GameObject* obj, UINT32& linkId) { obj->mLinkId = linkId; }

	public:
		/**
		 * @brief	Helper method used for creating Component objects used during deserialization.
		 */
		template <typename T>
		static std::shared_ptr<T> createGameObject()
		{
			SPtr<T> component = SceneObject::createEmptyComponent<T>();

			// Every GameObject must store GODeserializationData in its RTTI data field during deserialization
			component->mRTTIData = GODeserializationData();
			GODeserializationData& deserializationData = any_cast_ref<GODeserializationData>(component->mRTTIData);

			// Store shared pointer since the system only provides us with raw ones
			deserializationData.ptr = component;

			return component;
		}

	public:
		GameObjectRTTI()
		{
			addPlainField("mInstanceID", 0, &GameObjectRTTI::getInstanceID, &GameObjectRTTI::setInstanceID);
			addPlainField("mName", 1, &GameObjectRTTI::getName, &GameObjectRTTI::setName);
			addPlainField("mLinkId", 2, &GameObjectRTTI::getLinkId, &GameObjectRTTI::setLinkId);
		}

		const String& getRTTIName() override
		{
			static String name = "GameObject";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GameObject;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
			return nullptr;
		}
	};

	/** @} */
	/** @endcond */
}