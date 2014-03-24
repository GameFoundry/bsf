#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObjectManager.h"
#include "BsScriptSerializableArray.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArrayRTTI : public CM::RTTIType<ScriptSerializableArray, CM::IReflectable, ScriptSerializableArrayRTTI>
	{
	private:
		ScriptSerializableTypeInfoArrayPtr getTypeInfo(ScriptSerializableArray* obj)
		{
			return obj->mArrayTypeInfo;
		}

		void setTypeInfo(ScriptSerializableArray* obj, ScriptSerializableTypeInfoArrayPtr val)
		{
			obj->mArrayTypeInfo = val;
		}

		CM::UINT32& getNumElements(ScriptSerializableArray* obj)
		{
			return (CM::UINT32)obj->mNumElements;
		}

		void setNumElements(ScriptSerializableArray* obj, CM::UINT32& numElements)
		{
			obj->mNumElements = numElements;
		}

		ScriptSerializableFieldDataPtr getArrayEntry(ScriptSerializableArray* obj, CM::UINT32 arrayIdx)
		{
			return obj->mArrayEntries[arrayIdx];
		}

		void setArrayEntry(ScriptSerializableArray* obj, CM::UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mArrayEntries[arrayIdx] = val;
		}

		CM::UINT32 getNumArrayEntries(ScriptSerializableArray* obj)
		{
			return (CM::UINT32)obj->mArrayEntries.size();
		}

		void setNumArrayEntries(ScriptSerializableArray* obj, CM::UINT32 numEntries)
		{
			obj->mArrayEntries.resize(numEntries);
		}

	public:
		ScriptSerializableArrayRTTI()
		{
			addReflectablePtrField("mArrayTypeInfo", 0, &ScriptSerializableArrayRTTI::getTypeInfo, &ScriptSerializableArrayRTTI::setTypeInfo);
			addPlainField("mNumElements", 1, &ScriptSerializableArrayRTTI::getNumElements, &ScriptSerializableArrayRTTI::setNumElements);
			addReflectablePtrArrayField("mArrayEntries", 2, &ScriptSerializableArrayRTTI::getArrayEntry, &ScriptSerializableArrayRTTI::getNumArrayEntries, 
				&ScriptSerializableArrayRTTI::setArrayEntry, &ScriptSerializableArrayRTTI::setNumArrayEntries);
		}

		virtual void onSerializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableArray* serializableObject = static_cast<ScriptSerializableArray*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(CM::IReflectable* obj)
		{
			ScriptSerializableArray* serializableObject = static_cast<ScriptSerializableArray*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(CM::GameObjectManager::instance().isGameObjectDeserializationActive())
				CM::GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "ScriptSerializableArray";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_ScriptSerializableArray;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return ScriptSerializableArray::createEmpty();
		}
	};
}