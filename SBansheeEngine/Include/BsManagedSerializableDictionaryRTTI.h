#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectManager.h"
#include "BsManagedSerializableDictionary.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDictionaryRTTI : public RTTIType<ManagedSerializableDictionary, IReflectable, ManagedSerializableDictionaryRTTI>
	{
	private:
		struct SerializationData
		{
			Vector<ManagedSerializableFieldDataPtr> keyEntries;
			Vector<ManagedSerializableFieldDataPtr> valueEntries;
			bool isGameObjectDeserialization;
		};

		ManagedSerializableTypeInfoDictionaryPtr getTypeInfo(ManagedSerializableDictionary* obj) { return obj->mDictionaryTypeInfo; }
		void setTypeInfo(ManagedSerializableDictionary* obj, ManagedSerializableTypeInfoDictionaryPtr val) { obj->mDictionaryTypeInfo = val; }

		ManagedSerializableFieldDataPtr getKeyEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			return data->keyEntries[arrayIdx];
		}

		void setKeyEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			data->keyEntries[arrayIdx] = val;
		}

		UINT32 getNumKeyEntries(ManagedSerializableDictionary* obj) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			return (UINT32)data->keyEntries.size();
		}

		void setNumKeyEntries(ManagedSerializableDictionary* obj, UINT32 numEntries) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			data->keyEntries.resize(numEntries);
		}

		ManagedSerializableFieldDataPtr getValueEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			return data->valueEntries[arrayIdx];
		}

		void setValueEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			data->valueEntries[arrayIdx] = val;
		}

		UINT32 getNumValueEntries(ManagedSerializableDictionary* obj) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			return (UINT32)data->valueEntries.size();
		}

		void setNumValueEntries(ManagedSerializableDictionary* obj, UINT32 numEntries) 
		{ 
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(obj->mRTTIData);
			data->valueEntries.resize(numEntries);
		}

	public:
		ManagedSerializableDictionaryRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ManagedSerializableDictionaryRTTI::getTypeInfo, &ManagedSerializableDictionaryRTTI::setTypeInfo);
			addReflectablePtrArrayField("mKeyEntries", 1, &ManagedSerializableDictionaryRTTI::getKeyEntry, &ManagedSerializableDictionaryRTTI::getNumKeyEntries, 
				&ManagedSerializableDictionaryRTTI::setKeyEntry, &ManagedSerializableDictionaryRTTI::setNumKeyEntries);
			addReflectablePtrArrayField("mValueEntries", 2, &ManagedSerializableDictionaryRTTI::getValueEntry, &ManagedSerializableDictionaryRTTI::getNumValueEntries, 
				&ManagedSerializableDictionaryRTTI::setValueEntry, &ManagedSerializableDictionaryRTTI::setNumValueEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);

			serializableObject->mRTTIData = bs_shared_ptr<SerializationData>();
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(serializableObject->mRTTIData);

			serializableObject->serializeManagedInstance(data->keyEntries, data->valueEntries);
		}

		virtual void onSerializationEnded(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);
			serializableObject->mRTTIData = nullptr;
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);

			serializableObject->mRTTIData = bs_shared_ptr<SerializationData>();
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(serializableObject->mRTTIData);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			data->isGameObjectDeserialization = GameObjectManager::instance().isGameObjectDeserializationActive();

			if (data->isGameObjectDeserialization)
				GameObjectManager::instance().registerOnDeserializationEndCallback([=]() { serializableObject->deserializeManagedInstance(data->keyEntries, data->valueEntries); });
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);
			SPtr<SerializationData> data = any_cast<SPtr<SerializationData>>(serializableObject->mRTTIData);

			if (data->isGameObjectDeserialization)
				serializableObject->deserializeManagedInstance(data->keyEntries, data->valueEntries);

			serializableObject->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableDictionary";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableDictionary;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ManagedSerializableDictionary::createEmpty();
		}
	};
}