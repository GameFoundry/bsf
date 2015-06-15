#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsManagedSerializableDictionary.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDictionaryKeyValueRTTI : 
		public RTTIType <ManagedSerializableDictionaryKeyValue, IReflectable, ManagedSerializableDictionaryKeyValueRTTI>
	{
	private:
		ManagedSerializableFieldDataPtr getKey(ManagedSerializableDictionaryKeyValue* obj)
		{
			return obj->key;
		}

		void setKey(ManagedSerializableDictionaryKeyValue* obj, ManagedSerializableFieldDataPtr val)
		{
			obj->key = val;
		}

		ManagedSerializableFieldDataPtr getValue(ManagedSerializableDictionaryKeyValue* obj)
		{
			return obj->value;
		}

		void setValue(ManagedSerializableDictionaryKeyValue* obj, ManagedSerializableFieldDataPtr val)
		{
			obj->value = val;
		}

	public:
		ManagedSerializableDictionaryKeyValueRTTI()
		{
			addReflectablePtrField("key", 0, &ManagedSerializableDictionaryKeyValueRTTI::getKey, &ManagedSerializableDictionaryKeyValueRTTI::setKey);
			addReflectablePtrField("value", 1, &ManagedSerializableDictionaryKeyValueRTTI::getValue, &ManagedSerializableDictionaryKeyValueRTTI::setValue);
		}
		
		virtual const String& getRTTIName()
		{
			static String name = "ManagedSerializableDictionaryKeyValue";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableDictionaryKeyValue;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<ManagedSerializableDictionaryKeyValue>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableDictionaryRTTI : public RTTIType<ManagedSerializableDictionary, IReflectable, ManagedSerializableDictionaryRTTI>
	{
	private:
		ManagedSerializableTypeInfoDictionaryPtr getTypeInfo(ManagedSerializableDictionary* obj) { return obj->mDictionaryTypeInfo; }
		void setTypeInfo(ManagedSerializableDictionary* obj, ManagedSerializableTypeInfoDictionaryPtr val) { obj->mDictionaryTypeInfo = val; }

		ManagedSerializableDictionaryKeyValue& getEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx)
		{ 
			ManagedSerializableDictionaryPtr data = any_cast<ManagedSerializableDictionaryPtr>(obj->mRTTIData);
			Vector<ManagedSerializableDictionaryKeyValue>& sequentialData = any_cast_ref<Vector<ManagedSerializableDictionaryKeyValue>>(data->mRTTIData);

			return sequentialData[arrayIdx];
		}

		void setEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx, ManagedSerializableDictionaryKeyValue& val)
		{ 
			obj->setFieldData(val.key, val.value);
		}

		UINT32 getNumEntries(ManagedSerializableDictionary* obj) 
		{ 
			ManagedSerializableDictionaryPtr data = any_cast<ManagedSerializableDictionaryPtr>(obj->mRTTIData);
			Vector<ManagedSerializableDictionaryKeyValue>& sequentialData = any_cast_ref<Vector<ManagedSerializableDictionaryKeyValue>>(data->mRTTIData);

			return (UINT32)sequentialData.size();
		}

		void setNumEntries(ManagedSerializableDictionary* obj, UINT32 numEntries) 
		{ 
			// Do nothing
		}
		
	public:
		ManagedSerializableDictionaryRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ManagedSerializableDictionaryRTTI::getTypeInfo, &ManagedSerializableDictionaryRTTI::setTypeInfo);
			addReflectableArrayField("mEntries", 1, &ManagedSerializableDictionaryRTTI::getEntry, &ManagedSerializableDictionaryRTTI::getNumEntries,
				&ManagedSerializableDictionaryRTTI::setEntry, &ManagedSerializableDictionaryRTTI::setNumEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);

			Vector<ManagedSerializableDictionaryKeyValue> sequentialData;
			auto enumerator = serializableObject->getEnumerator();

			while (enumerator.moveNext())
				sequentialData.push_back(ManagedSerializableDictionaryKeyValue(enumerator.getKey(), enumerator.getValue()));

			serializableObject->mRTTIData = sequentialData;
		}

		virtual void onSerializationEnded(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);
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