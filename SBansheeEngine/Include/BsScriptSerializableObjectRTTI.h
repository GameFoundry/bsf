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
	class BS_SCR_BE_EXPORT ScriptSerializableObjectRTTI : public RTTIType<ScriptSerializableObject, IReflectable, ScriptSerializableObjectRTTI>
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

		ScriptSerializableFieldDataEntryPtr getFieldEntry(ScriptSerializableObject* obj, UINT32 arrayIdx)
		{
			return obj->mFieldEntries[arrayIdx];
		}

		void setFieldsEntry(ScriptSerializableObject* obj, UINT32 arrayIdx, ScriptSerializableFieldDataEntryPtr val)
		{
			obj->mFieldEntries[arrayIdx] = val;
		}

		UINT32 getNumFieldEntries(ScriptSerializableObject* obj)
		{
			return (UINT32)obj->mFieldEntries.size();
		}

		void setNumFieldEntries(ScriptSerializableObject* obj, UINT32 numEntries)
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

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ScriptSerializableObject* serializableObject = static_cast<ScriptSerializableObject*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ScriptSerializableObject* serializableObject = static_cast<ScriptSerializableObject*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(GameObjectManager::instance().isGameObjectDeserializationActive())
				GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ScriptSerializableObject::createEmpty();
		}
	};
}