#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsSceneObject.h"
#include "BsGameObjectHandle.h"
#include "BsGameObjectManager.h"
#include "BsComponent.h"

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

		HPrefab& getPrefabLink(SceneObject* obj) { return obj->mPrefabLink; }
		void setPrefabLink(SceneObject* obj, HPrefab& value) { obj->mPrefabLink = value; }

		UINT32& getFlags(SceneObject* obj) { return obj->mFlags; }
		void setFlags(SceneObject* obj, UINT32& value) { obj->mFlags = value; }
	public:
		SceneObjectRTTI()
		{
			addReflectablePtrArrayField("mChildren", 0, &SceneObjectRTTI::getChild, 
				&SceneObjectRTTI::getNumChildren, &SceneObjectRTTI::setChild, &SceneObjectRTTI::setNumChildren);
			addReflectablePtrArrayField("mComponents", 1, &SceneObjectRTTI::getComponent, 
				&SceneObjectRTTI::getNumComponents, &SceneObjectRTTI::setComponent, &SceneObjectRTTI::setNumComponents);
			addReflectableField("mPrefabLink", 2, &SceneObjectRTTI::getPrefabLink, &SceneObjectRTTI::setPrefabLink);
			addPlainField("mFlags", 3, &SceneObjectRTTI::getFlags, &SceneObjectRTTI::setFlags);
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