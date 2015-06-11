#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameObjectManager.h"
#include "BsManagedSerializableArray.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableArrayRTTI : public RTTIType<ManagedSerializableArray, IReflectable, ManagedSerializableArrayRTTI>
	{
	private:
		struct DeserializationInfo
		{
			Vector<ManagedSerializableFieldDataPtr> fields;
			bool isGameObjectDeserialization;
		};

		ManagedSerializableTypeInfoArrayPtr getTypeInfo(ManagedSerializableArray* obj)
		{
			return obj->mArrayTypeInfo;
		}

		void setTypeInfo(ManagedSerializableArray* obj, ManagedSerializableTypeInfoArrayPtr val)
		{
			obj->mArrayTypeInfo = val;
		}

		UINT32& getElementSize(ManagedSerializableArray* obj)
		{
			return (UINT32)obj->mElemSize;
		}

		void setElementSize(ManagedSerializableArray* obj, UINT32& numElements)
		{
			obj->mElemSize = numElements;
		}

		UINT32& getNumElements(ManagedSerializableArray* obj, UINT32 arrayIdx)
		{
			return (UINT32)obj->mNumElements[arrayIdx];
		}

		void setNumElements(ManagedSerializableArray* obj, UINT32 arrayIdx, UINT32& numElements)
		{
			obj->mNumElements[arrayIdx] = numElements;
		}

		UINT32 getNumElementsNumEntries(ManagedSerializableArray* obj)
		{
			return (UINT32)obj->mNumElements.size();
		}

		void setNumElementsNumEntries(ManagedSerializableArray* obj, UINT32 numEntries)
		{
			obj->mNumElements.resize(numEntries);
		}

		ManagedSerializableFieldDataPtr getArrayEntry(ManagedSerializableArray* obj, UINT32 arrayIdx)
		{
			return obj->getFieldData(arrayIdx);
		}

		void setArrayEntry(ManagedSerializableArray* obj, UINT32 arrayIdx, ManagedSerializableFieldDataPtr val)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields[arrayIdx] = val;
		}

		UINT32 getNumArrayEntries(ManagedSerializableArray* obj)
		{
			UINT32 totalNumElements = 1;
			for (auto& numElems : obj->mNumElements)
				totalNumElements *= numElems;

			return totalNumElements;
		}

		void setNumArrayEntries(ManagedSerializableArray* obj, UINT32 numEntries)
		{
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(obj->mRTTIData);
			info->fields = Vector<ManagedSerializableFieldDataPtr>(numEntries);
		}

	public:
		ManagedSerializableArrayRTTI()
		{
			addReflectablePtrField("mArrayTypeInfo", 0, &ManagedSerializableArrayRTTI::getTypeInfo, &ManagedSerializableArrayRTTI::setTypeInfo);
			addPlainField("mElementSize", 1, &ManagedSerializableArrayRTTI::getElementSize, &ManagedSerializableArrayRTTI::setElementSize);
			addPlainArrayField("mNumElements", 2, &ManagedSerializableArrayRTTI::getNumElements, &ManagedSerializableArrayRTTI::getNumElementsNumEntries, 
				&ManagedSerializableArrayRTTI::setNumElements, &ManagedSerializableArrayRTTI::setNumElementsNumEntries);
			addReflectablePtrArrayField("mArrayEntries", 3, &ManagedSerializableArrayRTTI::getArrayEntry, &ManagedSerializableArrayRTTI::getNumArrayEntries, 
				&ManagedSerializableArrayRTTI::setArrayEntry, &ManagedSerializableArrayRTTI::setNumArrayEntries);
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			ManagedSerializableArray* serializableObject = static_cast<ManagedSerializableArray*>(obj);

			serializableObject->mRTTIData = bs_shared_ptr<DeserializationInfo>();
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);

			// If we are deserializing a GameObject we need to defer deserializing actual field values
			// to ensure GameObject handles instances have been fixed up (which only happens after deserialization is done)
			info->isGameObjectDeserialization = GameObjectManager::instance().isGameObjectDeserializationActive();

			if (info->isGameObjectDeserialization)
				GameObjectManager::instance().registerOnDeserializationEndCallback([=]() { serializableObject->deserializeManagedInstance(info->fields); });
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			ManagedSerializableArray* serializableObject = static_cast<ManagedSerializableArray*>(obj);
			SPtr<DeserializationInfo> info = any_cast<SPtr<DeserializationInfo>>(serializableObject->mRTTIData);
			
			if (!info->isGameObjectDeserialization)
				serializableObject->deserializeManagedInstance(info->fields);

			serializableObject->mRTTIData = nullptr;
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
			return ManagedSerializableArray::createNew();
		}
	};
}