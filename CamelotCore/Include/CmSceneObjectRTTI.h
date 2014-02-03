#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmSceneObject.h"
#include "CmGameObjectHandle.h"
#include "CmGameObjectManager.h"

namespace CamelotFramework
{
	class CM_EXPORT SceneObjectRTTI : public RTTIType<SceneObject, GameObject, SceneObjectRTTI>
	{
	private:
		String& getName(SceneObject* obj) { return obj->mName; }
		void setName(SceneObject* obj, String& name) { obj->mName = name; }

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

	public:
		SceneObjectRTTI()
		{
			addPlainField("mName", 0, &SceneObjectRTTI::getName, &SceneObjectRTTI::setName);
			addReflectablePtrArrayField("mChildren", 1, &SceneObjectRTTI::getChild, 
				&SceneObjectRTTI::getNumChildren, &SceneObjectRTTI::setChild, &SceneObjectRTTI::setNumChildren);
			addReflectablePtrArrayField("mComponents", 2, &SceneObjectRTTI::getComponent, 
				&SceneObjectRTTI::getNumComponents, &SceneObjectRTTI::setComponent, &SceneObjectRTTI::setNumComponents);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SceneObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SceneObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			HSceneObject newObject = SceneObject::createInternal("");

			return newObject.getInternalPtr();
		}

	protected:
		virtual void onDeserializationStarted(IReflectable* obj)
		{
			SceneObject* sceneObj = static_cast<SceneObject*>(obj);
			GameObjectManager::instance().notifyDeserializationStarted(sceneObj);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			SceneObject* sceneObj = static_cast<SceneObject*>(obj);
			GameObjectManager::instance().notifyDeserializationEnded(sceneObj);
		}
	};
}