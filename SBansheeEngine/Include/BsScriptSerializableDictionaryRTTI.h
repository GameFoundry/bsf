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
		ScriptSerializableTypeInfoDictionaryPtr getTypeInfo(ScriptSerializableDictionary* obj)
		{
			return obj->mDictionaryTypeInfo;
		}

		void setTypeInfo(ScriptSerializableDictionary* obj, ScriptSerializableTypeInfoDictionaryPtr val)
		{
			obj->mDictionaryTypeInfo = val;
		}

		CM::UINT32& getNumElements(ScriptSerializableDictionary* obj)
		{
			return (CM::UINT32)obj->mNumElements;
		}

		void setNumElements(ScriptSerializableDictionary* obj, CM::UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ScriptSerializableFieldDataPtr getListEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx)
		{
			return obj->mDictionaryEntries[arrayIdx];
		}

		void setListEntry(ScriptSerializableDictionary* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mDictionaryEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumListEntries(ScriptSerializableDictionary* obj)
		{
			return (CM::UINT32)obj->mDictionaryEntries.size();
		}

		void setNumListEntries(ScriptSerializableDictionary* obj, CM::UINT32 numEntries)
		{
			obj->mDictionaryEntries.resize(numEntries);
		}

	public:
		ScriptSerializableDictionaryRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ScriptSerializableDictionaryRTTI::getTypeInfo, &ScriptSerializableDictionaryRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ScriptSerializableDictionaryRTTI::getNumElements, &ScriptSerializableDictionaryRTTI::setNumElements);
			addReflectablePtrArrayField("mListEntries", 2, &ScriptSerializableDictionaryRTTI::getListEntry, &ScriptSerializableDictionaryRTTI::getNumListEntries, 
				&ScriptSerializableDictionaryRTTI::setListEntry, &ScriptSerializableDictionaryRTTI::setNumListEntries);
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