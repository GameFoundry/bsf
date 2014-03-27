#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectManager.h"
#include "BsScriptSerializableDictionary.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableDictionaryRTTI : public CM::RTTIType<ScriptSerializableDictionary, CM::IReflectable, ScriptSerializableDictionaryRTTI>
	{
	private:
		ScriptSerializableTypeInfoDictionaryPtr getTypeInfo(ScriptSerializableDictionary* obj) { return obj->mDictionaryTypeInfo; }
		void setTypeInfo(ScriptSerializableDictionary* obj, ScriptSerializableTypeInfoDictionaryPtr val) { obj->mDictionaryTypeInfo = val; }

		ScriptSerializableFieldDataPtr getKeyEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx) { return obj->mKeyEntries[arrayIdx]; }
		void setKeyEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val) { obj->mKeyEntries[arrayIdx] = val; }
		CM::UINT32 getNumKeyEntries(ScriptSerializableDictionary* obj) { return (CM::UINT32)obj->mKeyEntries.size(); }
		void setNumKeyEntries(ScriptSerializableDictionary* obj, CM::UINT32 numEntries) { obj->mKeyEntries.resize(numEntries); }

		ScriptSerializableFieldDataPtr getValueEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx) { return obj->mKeyEntries[arrayIdx]; }
		void setValueEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val) { obj->mKeyEntries[arrayIdx] = val; }
		CM::UINT32 getNumValueEntries(ScriptSerializableDictionary* obj) { return (CM::UINT32)obj->mKeyEntries.size(); }
		void setNumValueEntries(ScriptSerializableDictionary* obj, CM::UINT32 numEntries) { obj->mKeyEntries.resize(numEntries); }

	public:
		ScriptSerializableDictionaryRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ScriptSerializableDictionaryRTTI::getTypeInfo, &ScriptSerializableDictionaryRTTI::setTypeInfo);
			addReflectablePtrArrayField("mKeyEntries", 1, &ScriptSerializableDictionaryRTTI::getKeyEntry, &ScriptSerializableDictionaryRTTI::getNumKeyEntries, 
				&ScriptSerializableDictionaryRTTI::setKeyEntry, &ScriptSerializableDictionaryRTTI::setNumKeyEntries);
			addReflectablePtrArrayField("mValueEntries", 2, &ScriptSerializableDictionaryRTTI::getValueEntry, &ScriptSerializableDictionaryRTTI::getNumValueEntries, 
				&ScriptSerializableDictionaryRTTI::setValueEntry, &ScriptSerializableDictionaryRTTI::setNumValueEntries);
		}

		virtual void onSerializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableDictionary* serializableObject = static_cast<ScriptSerializableDictionary*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableDictionary* serializableObject = static_cast<ScriptSerializableDictionary*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(CM::GameObjectManager::instance().isGameObjectDeserializationActive())
				CM::GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableDictionary";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableDictionary;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableDictionary::createEmpty();
		}
	};
}