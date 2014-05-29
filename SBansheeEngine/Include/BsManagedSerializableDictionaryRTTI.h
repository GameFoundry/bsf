#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectManager.h"
#include "BsManagedSerializableDictionary.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDictionaryRTTI : public RTTIType<ManagedSerializableDictionary, IReflectable, ManagedSerializableDictionaryRTTI>
	{
	private:
		ManagedSerializableTypeInfoDictionaryPtr getTypeInfo(ManagedSerializableDictionary* obj) { return obj->mDictionaryTypeInfo; }
		void setTypeInfo(ManagedSerializableDictionary* obj, ManagedSerializableTypeInfoDictionaryPtr val) { obj->mDictionaryTypeInfo = val; }

		ManagedSerializableFieldDataPtr getKeyEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx) { return obj->mKeyEntries[arrayIdx]; }
		void setKeyEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val) { obj->mKeyEntries[arrayIdx] = val; }
		UINT32 getNumKeyEntries(ManagedSerializableDictionary* obj) { return (UINT32)obj->mKeyEntries.size(); }
		void setNumKeyEntries(ManagedSerializableDictionary* obj, UINT32 numEntries) { obj->mKeyEntries.resize(numEntries); }

		ManagedSerializableFieldDataPtr getValueEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx) { return obj->mValueEntries[arrayIdx]; }
		void setValueEntry(ManagedSerializableDictionary* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val) { obj->mValueEntries[arrayIdx] = val; }
		UINT32 getNumValueEntries(ManagedSerializableDictionary* obj) { return (UINT32)obj->mValueEntries.size(); }
		void setNumValueEntries(ManagedSerializableDictionary* obj, UINT32 numEntries) { obj->mValueEntries.resize(numEntries); }

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
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableDictionary* serializableObject = static_cast<ManagedSerializableDictionary*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(GameObjectManager::instance().isGameObjectDeserializationActive())
				GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
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