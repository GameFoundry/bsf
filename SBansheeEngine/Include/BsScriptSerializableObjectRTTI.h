#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableField.h"
#include "CmGameObjectManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObjectRTTI : public CM::RTTIType<ScriptSerializableObject, CM::IReflectable, ScriptSerializableObjectRTTI>
	{
	private:
		ScriptSerializableObjectInfoPtr getInfo(ScriptSerializableObject* obj)
		{
			return obj->mObjInfo;
		}

		void setInfo(ScriptSerializableObject* obj, ScriptSerializableObjectInfoPtr val)
		{
			obj->mObjInfo = val;
		}

		ScriptSerializableFieldDataEntryPtr getFieldEntry(ScriptSerializableObject* obj, CM::UINT32 arrayIdx)
		{
			return obj->mFieldEntries[arrayIdx];
		}

		void setFieldsEntry(ScriptSerializableObject* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataEntryPtr val)
		{
			obj->mFieldEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumFieldEntries(ScriptSerializableObject* obj)
		{
			return (CM::UINT32)obj->mFieldEntries.size();
		}

		void setNumFieldEntries(ScriptSerializableObject* obj, CM::UINT32 numEntries)
		{
			obj->mFieldEntries.resize(numEntries);
		}

	public:
		ScriptSerializableObjectRTTI()
		{
			addReflectablePtrField("mObjInfo", 0, &ScriptSerializableObjectRTTI::getInfo, &ScriptSerializableObjectRTTI::setInfo);
			addReflectablePtrArrayField("mFieldEntries", 1, &ScriptSerializableObjectRTTI::getFieldEntry, &ScriptSerializableObjectRTTI::getNumFieldEntries, 
				&ScriptSerializableObjectRTTI::setFieldsEntry, &ScriptSerializableObjectRTTI::setNumFieldEntries);
		}

		virtual void onSerializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableObject* serializableObject = static_cast<ScriptSerializableObject*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableObject* serializableObject = static_cast<ScriptSerializableObject*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(CM::GameObjectManager::instance().isGameObjectDeserializationActive())
				CM::GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableObject";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableObject;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableObject::createEmpty();
		}
	};
}