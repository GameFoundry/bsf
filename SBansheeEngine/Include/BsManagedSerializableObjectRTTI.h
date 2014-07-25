#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsRuntimeScriptObjects.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableField.h"
#include "BsGameObjectManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableObjectRTTI : public RTTIType<ManagedSerializableObject, IReflectable, ManagedSerializableObjectRTTI>
	{
	private:
		ManagedSerializableObjectInfoPtr getInfo(ManagedSerializableObject* obj)
		{
			return obj->mObjInfo;
		}

		void setInfo(ManagedSerializableObject* obj, ManagedSerializableObjectInfoPtr val)
		{
			obj->mObjInfo = val;
		}

		ManagedSerializableFieldDataEntryPtr getFieldEntry(ManagedSerializableObject* obj, UINT32 arrayIdx)
		{
			return obj->mFieldEntries[arrayIdx];
		}

		void setFieldsEntry(ManagedSerializableObject* obj, UINT32 arrayIdx, ManagedSerializableFieldDataEntryPtr val)
		{
			obj->mFieldEntries[arrayIdx] = val;
		}

		UINT32 getNumFieldEntries(ManagedSerializableObject* obj)
		{
			return (UINT32)obj->mFieldEntries.size();
		}

		void setNumFieldEntries(ManagedSerializableObject* obj, UINT32 numEntries)
		{
			obj->mFieldEntries.resize(numEntries);
		}

	public:
		ManagedSerializableObjectRTTI()
		{
			addReflectablePtrField("mObjInfo", 0, &ManagedSerializableObjectRTTI::getInfo, &ManagedSerializableObjectRTTI::setInfo);
			addReflectablePtrArrayField("mFieldEntries", 1, &ManagedSerializableObjectRTTI::getFieldEntry, &ManagedSerializableObjectRTTI::getNumFieldEntries, 
				&ManagedSerializableObjectRTTI::setFieldsEntry, &ManagedSerializableObjectRTTI::setNumFieldEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ManagedSerializableObject* serializableObject = static_cast<ManagedSerializableObject*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableObject* serializableObject = static_cast<ManagedSerializableObject*>(obj);
			
			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if (GameObjectManager::instance().isGameObjectDeserializationActive())
			{
				GameObjectManager::instance().registerOnDeserializationEndCallback([=]() { serializableObject->deserializeManagedInstance(); });
				serializableObject->mRTTIData = true;
			}
			else
				serializableObject->mRTTIData = false;
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedSerializableObject* serializableObject = static_cast<ManagedSerializableObject*>(obj);

			bool isGameObjectDeserialization = any_cast<bool>(serializableObject->mRTTIData);
			if (!isGameObjectDeserialization)
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
			return ManagedSerializableObject::createEmpty();
		}
	};
}