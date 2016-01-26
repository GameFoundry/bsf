#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectManager.h"
#include "BsManagedSerializableList.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableListRTTI : public RTTIType<ManagedSerializableList, IReflectable, ManagedSerializableListRTTI>
	{
	private:
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
			return obj->mListEntries[arrayIdx];
		}

		void setListEntry(ManagedSerializableList* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val)
		{
			obj->mListEntries[arrayIdx] = val;
		}

		UINT32 getNumListEntries(ManagedSerializableList* obj)
		{
			return (UINT32)obj->mListEntries.size();
		}

		void setNumListEntries(ManagedSerializableList* obj, UINT32 numEntries)
		{
			obj->mListEntries.resize(numEntries);
		}

	public:
		ManagedSerializableListRTTI()
		{
			addReflectablePtrField("mListTypeInfo", 0, &ManagedSerializableListRTTI::getTypeInfo, &ManagedSerializableListRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ManagedSerializableListRTTI::getNumElements, &ManagedSerializableListRTTI::setNumElements);
			addReflectablePtrArrayField("mListEntries", 2, &ManagedSerializableListRTTI::getListEntry, &ManagedSerializableListRTTI::getNumListEntries, 
				&ManagedSerializableListRTTI::setListEntry, &ManagedSerializableListRTTI::setNumListEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ManagedSerializableList* serializableObject = static_cast<ManagedSerializableList*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableList* serializableObject = static_cast<ManagedSerializableList*>(obj);

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
			return ManagedSerializableList::createEmpty();
		}
	};
}