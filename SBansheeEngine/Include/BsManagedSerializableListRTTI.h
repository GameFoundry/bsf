#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectManager.h"
#include "BsManagedSerializableList.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableListRTTI : public RTTIType<ManagedSerializableList, IReflectable, ManagedSerializableListRTTI>
	{
	private:
		struct DeserializationInfo
		{
			Vector<ManagedSerializableFieldDataPtr> fields;
			bool isGameObjectDeserialization;
		};

		ManagedSerializableTypeInfoListPtr getTypeInfo(ManagedSerializableList* obj)
		{
			return obj->mListTypeInfo;
		}

		void setTypeInfo(ManagedSerializableList* obj, ManagedSerializableTypeInfoListPtr val)
		{
			obj->mListTypeInfo = val;
		}

		UINT32& getNumElements(ManagedSerializableList* obj)
		{
			return (UINT32)obj->mNumElements;
		}

		void setNumElements(ManagedSerializableList* obj, UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ManagedSerializableFieldDataPtr getListEntry(ManagedSerializableList* obj, UINT32 arrayIdx)
		{
			return obj->getFieldData(arrayIdx);
		}

		void setListEntry(ManagedSerializableList* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields[arrayIdx] = val;
		}

		UINT32 getNumListEntries(ManagedSerializableList* obj)
		{
			return obj->mNumElements;
		}

		void setNumListEntries(ManagedSerializableList* obj, UINT32 numEntries)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields = Vector<ManagedSerializableFieldDataPtr>(numEntries);
		}

	public:
		ManagedSerializableListRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ManagedSerializableListRTTI::getTypeInfo, &ManagedSerializableListRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ManagedSerializableListRTTI::getNumElements, &ManagedSerializableListRTTI::setNumElements);
			addReflectablePtrArrayField("mListEntries", 2, &ManagedSerializableListRTTI::getListEntry, &ManagedSerializableListRTTI::getNumListEntries, 
				&ManagedSerializableListRTTI::setListEntry, &ManagedSerializableListRTTI::setNumListEntries);
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableList* serializableObject = static_cast<ManagedSerializableList*>(obj);

			serializableObject->mRTTIData = bs_shared_ptr<DeserializationInfo>();
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			info->isGameObjectDeserialization = GameObjectManager::instance().isGameObjectDeserializationActive();

			if (info->isGameObjectDeserialization)
				GameObjectManager::instance().registerOnDeserializationEndCallback([=]() { serializableObject->deserializeManagedInstance(info->fields); });
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedSerializableList* serializableObject = static_cast<ManagedSerializableList*>(obj);
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);

			if (!info->isGameObjectDeserialization)
				serializableObject->deserializeManagedInstance(info->fields);

			serializableObject->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableList";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableList;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ManagedSerializableList::createEmpty();
		}
	};
}