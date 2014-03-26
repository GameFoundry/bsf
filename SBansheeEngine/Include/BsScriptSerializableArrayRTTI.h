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

		CM::UINT32& getElementSize(ScriptSerializableArray* obj)
		{
			return (CM::UINT32)obj->mElemSize;
		}

		void setElementSize(ScriptSerializableArray* obj, CM::UINT32& numElements)
		{
			obj->mElemSize = numElements;
		}

		CM::UINT32& getNumElements(ScriptSerializableArray* obj, CM::UINT32 arrayIdx)
		{
			return (CM::UINT32)obj->mNumElements[arrayIdx];
		}

		void setNumElements(ScriptSerializableArray* obj, CM::UINT32 arrayIdx, CM::UINT32& numElements)
		{
			obj->mNumElements[arrayIdx] = numElements;
		}

		CM::UINT32 getNumElementsNumEntries(ScriptSerializableArray* obj)
		{
			return (CM::UINT32)obj->mNumElements.size();
		}

		void setNumElementsNumEntries(ScriptSerializableArray* obj, CM::UINT32 numEntries)
		{
			obj->mNumElements.resize(numEntries);
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
			addPlainField("mElementSize", 1, &ScriptSerializableArrayRTTI::getElementSize, &ScriptSerializableArrayRTTI::setElementSize);
			addPlainArrayField("mNumElements", 2, &ScriptSerializableArrayRTTI::getNumElements, &ScriptSerializableArrayRTTI::getNumElementsNumEntries, 
				&ScriptSerializableArrayRTTI::setNumElements, &ScriptSerializableArrayRTTI::setNumElementsNumEntries);
			addReflectablePtrArrayField("mArrayEntries", 3, &ScriptSerializableArrayRTTI::getArrayEntry, &ScriptSerializableArrayRTTI::getNumArrayEntries, 
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