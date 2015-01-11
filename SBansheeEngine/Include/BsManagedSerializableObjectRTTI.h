#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableField.h"
#include "BsGameObjectManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableObjectRTTI : public RTTIType<ManagedSerializableObject, IReflectable, ManagedSerializableObjectRTTI>
	{
	private:
		struct DeserializationInfo
		{
			Vector<ManagedSerializableFieldDataEntryPtr> fields;
			bool isGameObjectDeserialization;
		};

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
			ManagedSerializableObjectInfo::CachedField field = obj->mObjInfo->mCachedAllFields[arrayIdx];

			ManagedSerializableFieldKeyPtr fieldKey = ManagedSerializableFieldKey::create(field.parentTypeId, field.info->mFieldId);
			ManagedSerializableFieldDataPtr fieldData = obj->getFieldData(field.info);

			return ManagedSerializableFieldDataEntry::create(fieldKey, fieldData);
		}

		void setFieldsEntry(ManagedSerializableObject* obj, UINT32 arrayIdx, ManagedSerializableFieldDataEntryPtr val)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields[arrayIdx] = val;
		}

		UINT32 getNumFieldEntries(ManagedSerializableObject* obj)
		{
			return (UINT32)obj->mObjInfo->mCachedAllFields.size();
		}

		void setNumFieldEntries(ManagedSerializableObject* obj, UINT32 numEntries)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields = Vector<ManagedSerializableFieldDataEntryPtr>(numEntries);
		}

	public:
		ManagedSerializableObjectRTTI()
		{
			addReflectablePtrField("mObjInfo", 0, &ManagedSerializableObjectRTTI::getInfo, &ManagedSerializableObjectRTTI::setInfo);
			addReflectablePtrArrayField("mFieldEntries", 1, &ManagedSerializableObjectRTTI::getFieldEntry, &ManagedSerializableObjectRTTI::getNumFieldEntries, 
				&ManagedSerializableObjectRTTI::setFieldsEntry, &ManagedSerializableObjectRTTI::setNumFieldEntries);
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableObject* serializableObject = static_cast<ManagedSerializableObject*>(obj);
			serializableObject->mRTTIData = bs_shared_ptr<DeserializationInfo>();

			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if (GameObjectManager::instance().isGameObjectDeserializationActive())
			{
				GameObjectManager::instance().registerOnDeserializationEndCallback([=]() { serializableObject->deserializeManagedInstance(info->fields); });
				info->isGameObjectDeserialization = true;
			}
			else
				info->isGameObjectDeserialization = false;
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedSerializableObject* serializableObject = static_cast<ManagedSerializableObject*>(obj);
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);

			if (!info->isGameObjectDeserialization)
				serializableObject->deserializeManagedInstance(info->fields);

			serializableObject->mRTTIData = nullptr;
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