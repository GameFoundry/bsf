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
	class BS_SCR_BE_EXPORT ScriptSerializableListRTTI : public RTTIType<ScriptSerializableList, IReflectable, ScriptSerializableListRTTI>
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

		UINT32& getNumElements(ScriptSerializableList* obj)
		{
			return (UINT32)obj->mNumElements;
		}

		void setNumElements(ScriptSerializableList* obj, UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ScriptSerializableFieldDataPtr getListEntry(ScriptSerializableList* obj, UINT32 arrayIdx)
		{
			return obj->mListEntries[arrayIdx];
		}

		void setListEntry(ScriptSerializableList* obj, UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mListEntries[arrayIdx] = val;
		}

		UINT32 getNumListEntries(ScriptSerializableList* obj)
		{
			return (UINT32)obj->mListEntries.size();
		}

		void setNumListEntries(ScriptSerializableList* obj, UINT32 numEntries)
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

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ScriptSerializableList* serializableObject = static_cast<ScriptSerializableList*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ScriptSerializableList* serializableObject = static_cast<ScriptSerializableList*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(GameObjectManager::instance().isGameObjectDeserializationActive())
				GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
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
			return ScriptSerializableList::createEmpty();
		}
	};
}