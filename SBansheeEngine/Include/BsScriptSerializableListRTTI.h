#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectManager.h"
#include "BsScriptSerializableList.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableListRTTI : public CM::RTTIType<ScriptSerializableList, CM::IReflectable, ScriptSerializableListRTTI>
	{
	private:
		ScriptSerializableTypeInfoListPtr getTypeInfo(ScriptSerializableList* obj)
		{
			return obj->mListTypeInfo;
		}

		void setTypeInfo(ScriptSerializableList* obj, ScriptSerializableTypeInfoListPtr val)
		{
			obj->mListTypeInfo = val;
		}

		CM::UINT32& getNumElements(ScriptSerializableList* obj)
		{
			return (CM::UINT32)obj->mNumElements;
		}

		void setNumElements(ScriptSerializableList* obj, CM::UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ScriptSerializableFieldDataPtr getListEntry(ScriptSerializableList* obj, CM::UINT32 arrayIdx)
		{
			return obj->mListEntries[arrayIdx];
		}

		void setListEntry(ScriptSerializableList* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mListEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumListEntries(ScriptSerializableList* obj)
		{
			return (CM::UINT32)obj->mListEntries.size();
		}

		void setNumListEntries(ScriptSerializableList* obj, CM::UINT32 numEntries)
		{
			obj->mListEntries.resize(numEntries);
		}

	public:
		ScriptSerializableListRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ScriptSerializableListRTTI::getTypeInfo, &ScriptSerializableListRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ScriptSerializableListRTTI::getNumElements, &ScriptSerializableListRTTI::setNumElements);
			addReflectablePtrArrayField("mListEntries", 2, &ScriptSerializableListRTTI::getListEntry, &ScriptSerializableListRTTI::getNumListEntries, 
				&ScriptSerializableListRTTI::setListEntry, &ScriptSerializableListRTTI::setNumListEntries);
		}

		virtual void onSerializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableList* serializableObject = static_cast<ScriptSerializableList*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableList* serializableObject = static_cast<ScriptSerializableList*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(CM::GameObjectManager::instance().isGameObjectDeserializationActive())
				CM::GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableList";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableList;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableList::createEmpty();
		}
	};
}