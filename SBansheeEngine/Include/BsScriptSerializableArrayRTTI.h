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
	class BS_SCR_BE_EXPORT ScriptSerializableArrayRTTI : public RTTIType<ScriptSerializableArray, IReflectable, ScriptSerializableArrayRTTI>
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

		UINT32& getElementSize(ScriptSerializableArray* obj)
		{
			return (UINT32)obj->mElemSize;
		}

		void setElementSize(ScriptSerializableArray* obj, UINT32& numElements)
		{
			obj->mElemSize = numElements;
		}

		UINT32& getNumElements(ScriptSerializableArray* obj, UINT32 arrayIdx)
		{
			return (UINT32)obj->mNumElements[arrayIdx];
		}

		void setNumElements(ScriptSerializableArray* obj, UINT32 arrayIdx, UINT32& numElements)
		{
			obj->mNumElements[arrayIdx] = numElements;
		}

		UINT32 getNumElementsNumEntries(ScriptSerializableArray* obj)
		{
			return (UINT32)obj->mNumElements.size();
		}

		void setNumElementsNumEntries(ScriptSerializableArray* obj, UINT32 numEntries)
		{
			obj->mNumElements.resize(numEntries);
		}

		ScriptSerializableFieldDataPtr getArrayEntry(ScriptSerializableArray* obj, UINT32 arrayIdx)
		{
			return obj->mArrayEntries[arrayIdx];
		}

		void setArrayEntry(ScriptSerializableArray* obj, UINT32 arrayIdx, ScriptSerializableFieldDataPtr val)
		{
			obj->mArrayEntries[arrayIdx] = val;
		}

		UINT32 getNumArrayEntries(ScriptSerializableArray* obj)
		{
			return (UINT32)obj->mArrayEntries.size();
		}

		void setNumArrayEntries(ScriptSerializableArray* obj, UINT32 numEntries)
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

		virtual void onSerializationStarted(IReflectable* obj)
		{
			ScriptSerializableArray* serializableObject = static_cast<ScriptSerializableArray*>(obj);
			serializableObject->serializeManagedInstance();
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ScriptSerializableArray* serializableObject = static_cast<ScriptSerializableArray*>(obj);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			if(GameObjectManager::instance().isGameObjectDeserializationActive())
				GameObjectManager::instance().registerOnDeserializationEndCallback([=] () { serializableObject->deserializeManagedInstance(); });
			else
				serializableObject->deserializeManagedInstance();
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptSerializableArray";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptSerializableArray;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ScriptSerializableArray::createEmpty();
		}
	};
}