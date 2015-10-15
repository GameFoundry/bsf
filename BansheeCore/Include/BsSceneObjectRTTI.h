#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsSceneObject.h"
#include "BsGameObjectHandle.h"
#include "BsGameObjectManager.h"
#include "BsComponent.h"
#include "BsPrefabDiff.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT SceneObjectRTTI : public RTTIType<SceneObject, GameObject, SceneObjectRTTI>
	{
	private:
		struct DeserializationData
		{
			DeserializationData(bool isDeserializationParent)
				:isDeserializationParent(isDeserializationParent)
			{ }

			bool isDeserializationParent;
		};

		Vector3& getPosition(SceneObject* obj) { return obj->mPosition; }
		void setPosition(SceneObject* obj, Vector3& value) { obj->mPosition = value; }

		Quaternion& getRotation(SceneObject* obj) { return obj->mRotation; }
		void setRotation(SceneObject* obj, Quaternion& value) { obj->mRotation = value; }

		Vector3& getScale(SceneObject* obj) { return obj->mScale; }
		void setScale(SceneObject* obj, Vector3& value) { obj->mScale = value; }

		bool& getActive(SceneObject* obj) { return obj->mActiveSelf; }
		void setActive(SceneObject* obj, bool& value) { obj->mActiveSelf = value; }

		// NOTE - These can only be set sequentially, specific array index is ignored
		std::shared_ptr<SceneObject> getChild(SceneObject* obj, UINT32 idx) { return obj->mChildren[idx].getInternalPtr(); }
		void setChild(SceneObject* obj, UINT32 idx, std::shared_ptr<SceneObject> param) { param->setParent(obj->mThisHandle); } // NOTE: Can only be used for sequentially adding elements
		UINT32 getNumChildren(SceneObject* obj) { return (UINT32)obj->mChildren.size(); }
		void setNumChildren(SceneObject* obj, UINT32 size) { /* DO NOTHING */ }

		// NOTE - These can only be set sequentially, specific array index is ignored
		std::shared_ptr<Component> getComponent(SceneObject* obj, UINT32 idx) { return obj->mComponents[idx].getInternalPtr(); }
		void setComponent(SceneObject* obj, UINT32 idx, std::shared_ptr<Component> param) { obj->addComponentInternal(param); }
		UINT32 getNumComponents(SceneObject* obj) { return (UINT32)obj->mComponents.size(); }
		void setNumComponents(SceneObject* obj, UINT32 size) { /* DO NOTHING */ }

		String& getPrefabLink(SceneObject* obj) { return obj->mPrefabLinkUUID; }
		void setPrefabLink(SceneObject* obj, String& value) { obj->mPrefabLinkUUID = value; }

		PrefabDiffPtr getPrefabDiff(SceneObject* obj) { return obj->mPrefabDiff; }
		void setPrefabDiff(SceneObject* obj, PrefabDiffPtr value) { obj->mPrefabDiff = value; }

		UINT32& getFlags(SceneObject* obj) { return obj->mFlags; }
		void setFlags(SceneObject* obj, UINT32& value) { obj->mFlags = value; }

		UINT32& getPrefabHash(SceneObject* obj) { return obj->mPrefabHash; }
		void setPrefabHash(SceneObject* obj, UINT32& value) { obj->mPrefabHash = value; }
	public:
		SceneObjectRTTI()
		{
			addReflectablePtrArrayField("mChildren", 0, &SceneObjectRTTI::getChild, 
				&SceneObjectRTTI::getNumChildren, &SceneObjectRTTI::setChild, &SceneObjectRTTI::setNumChildren);
			addReflectablePtrArrayField("mComponents", 1, &SceneObjectRTTI::getComponent, 
				&SceneObjectRTTI::getNumComponents, &SceneObjectRTTI::setComponent, &SceneObjectRTTI::setNumComponents);
			addPlainField("mPrefabLink", 2, &SceneObjectRTTI::getPrefabLink, &SceneObjectRTTI::setPrefabLink);
			addPlainField("mFlags", 3, &SceneObjectRTTI::getFlags, &SceneObjectRTTI::setFlags);
			addReflectablePtrField("mPrefabDiff", 4, &SceneObjectRTTI::getPrefabDiff, &SceneObjectRTTI::setPrefabDiff);
			addPlainField("mPrefabHash", 5, &SceneObjectRTTI::getPrefabHash, &SceneObjectRTTI::setPrefabHash);
			addPlainField("mPosition", 6, &SceneObjectRTTI::getPosition, &SceneObjectRTTI::setPosition);
			addPlainField("mRotation", 7, &SceneObjectRTTI::getRotation, &SceneObjectRTTI::setRotation);
			addPlainField("mScale", 8, &SceneObjectRTTI::getScale, &SceneObjectRTTI::setScale);
			addPlainField("mActiveSelf", 9, &SceneObjectRTTI::getActive, &SceneObjectRTTI::setActive);
		}

		virtual void onDeserializationStarted(IReflectable* obj) override
		{
			SceneObject* so = static_cast<SceneObject*>(obj);

			if (!GameObjectManager::instance().isGameObjectDeserializationActive())
			{
				GameObjectManager::instance().startDeserialization();
				so->mRTTIData = DeserializationData(true);
			}
			else
				so->mRTTIData = DeserializationData(false);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			SceneObject* so = static_cast<SceneObject*>(obj);
			DeserializationData deserializationData = any_cast<DeserializationData>(so->mRTTIData);

			if (deserializationData.isDeserializationParent)
			{
				GameObjectManager::instance().endDeserialization();

				if ((so->mFlags & SOF_DontInstantiate) == 0)
					so->instantiate();
			}

			so->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SceneObject";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SceneObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			HSceneObject newObject = SceneObject::create("", SOF_DontInstantiate);

			return newObject.getInternalPtr();
		}
	};
}