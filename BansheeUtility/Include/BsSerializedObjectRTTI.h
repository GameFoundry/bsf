#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsRTTIType.h"
#include "BsSerializedObject.h"

namespace BansheeEngine
{
	/** @cond RTTI */

	class BS_UTILITY_EXPORT SerializedInstanceRTTI : public RTTIType <SerializedInstance, IReflectable, SerializedInstanceRTTI>
	{
	public:
		SerializedInstanceRTTI()
		{ }

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedInstance";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedInstance;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return nullptr;
		}
	};

	class BS_UTILITY_EXPORT SerializedFieldRTTI : public RTTIType <SerializedField, SerializedInstance, SerializedFieldRTTI>
	{
	private:
		ManagedDataBlock getData(SerializedField* obj)
		{
			ManagedDataBlock dataBlock((UINT8*)obj->value, obj->size);
			return dataBlock;
		}

		void setData(SerializedField* obj, ManagedDataBlock val)
		{
			// Nothing to do here, the pointer we provided already belongs to SerializedField
			// so the data is already written
		}

		static UINT8* allocateData(SerializedField* obj, UINT32 numBytes)
		{
			obj->value = (UINT8*)bs_alloc(numBytes);
			obj->size = numBytes;
			obj->ownsMemory = true;

			return obj->value;
		}
	public:
		SerializedFieldRTTI()
		{
			addDataBlockField("data", 0, &SerializedFieldRTTI::getData, &SerializedFieldRTTI::setData, 0, &SerializedFieldRTTI::allocateData);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedField";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedField;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedField>();
		}
	};

	class BS_UTILITY_EXPORT SerializedObjectRTTI : public RTTIType <SerializedObject, SerializedInstance, SerializedObjectRTTI>
	{
	private:
		SerializedSubObject& getEntry(SerializedObject* obj, UINT32 arrayIdx)
		{
			return obj->subObjects[arrayIdx];
		}

		void setEntry(SerializedObject* obj, UINT32 arrayIdx, SerializedSubObject& val)
		{
			obj->subObjects[arrayIdx] = val;
		}

		UINT32 getNumEntries(SerializedObject* obj)
		{
			return (UINT32)obj->subObjects.size();
		}

		void setNumEntries(SerializedObject* obj, UINT32 numEntries)
		{
			obj->subObjects = Vector<SerializedSubObject>(numEntries);
		}
	public:
		SerializedObjectRTTI()
		{
			addReflectableArrayField("entries", 1, &SerializedObjectRTTI::getEntry, &SerializedObjectRTTI::getNumEntries,
				&SerializedObjectRTTI::setEntry, &SerializedObjectRTTI::setNumEntries);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedObject";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedObject>();
		}
	};

	class BS_UTILITY_EXPORT SerializedArrayRTTI : public RTTIType <SerializedArray, SerializedInstance, SerializedArrayRTTI>
	{
	private:
		UINT32& getNumElements(SerializedArray* obj)
		{
			return obj->numElements;
		}

		void setNumElements(SerializedArray* obj, UINT32& val)
		{
			obj->numElements = val;
		}

		SerializedArrayEntry& getEntry(SerializedArray* obj, UINT32 arrayIdx)
		{
			Vector<SerializedArrayEntry>& sequentialEntries = any_cast_ref<Vector<SerializedArrayEntry>>(obj->mRTTIData);
			return sequentialEntries[arrayIdx];
		}

		void setEntry(SerializedArray* obj, UINT32 arrayIdx, SerializedArrayEntry& val)
		{
			obj->entries[val.index] = val;
		}

		UINT32 getNumEntries(SerializedArray* obj)
		{
			Vector<SerializedArrayEntry>& sequentialEntries = any_cast_ref<Vector<SerializedArrayEntry>>(obj->mRTTIData);
			return (UINT32)sequentialEntries.size();
		}

		void setNumEntries(SerializedArray* obj, UINT32 numEntries)
		{
			obj->entries = UnorderedMap<UINT32, SerializedArrayEntry>();
		}
	public:
		SerializedArrayRTTI()
		{
			addPlainField("numElements", 0, &SerializedArrayRTTI::getNumElements, &SerializedArrayRTTI::setNumElements);
			addReflectableArrayField("entries", 1, &SerializedArrayRTTI::getEntry, &SerializedArrayRTTI::getNumEntries,
				&SerializedArrayRTTI::setEntry, &SerializedArrayRTTI::setNumEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj)
		{
			SerializedArray* serializedArray = static_cast<SerializedArray*>(obj);

			Vector<SerializedArrayEntry> sequentialData;
			for (auto& entry : serializedArray->entries)
				sequentialData.push_back(entry.second);

			serializedArray->mRTTIData = sequentialData;
		}

		virtual void onSerializationEnded(IReflectable* obj)
		{
			SerializedArray* serializedArray = static_cast<SerializedArray*>(obj);
			serializedArray->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedArray";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedArray;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedArray>();
		}
	};

	class BS_UTILITY_EXPORT SerializedSubObjectRTTI : public RTTIType <SerializedSubObject, IReflectable, SerializedSubObjectRTTI>
	{
	private:
		UINT32& getTypeId(SerializedSubObject* obj)
		{
			return obj->typeId;
		}

		void setTypeId(SerializedSubObject* obj, UINT32& val)
		{
			obj->typeId = val;
		}

		SerializedEntry& getEntry(SerializedSubObject* obj, UINT32 arrayIdx)
		{
			Vector<SerializedEntry>& sequentialEntries = any_cast_ref<Vector<SerializedEntry>>(obj->mRTTIData);
			return sequentialEntries[arrayIdx];
		}

		void setEntry(SerializedSubObject* obj, UINT32 arrayIdx, SerializedEntry& val)
		{
			obj->entries[val.fieldId] = val;
		}

		UINT32 getNumEntries(SerializedSubObject* obj)
		{
			Vector<SerializedEntry>& sequentialEntries = any_cast_ref<Vector<SerializedEntry>>(obj->mRTTIData);
			return (UINT32)sequentialEntries.size();
		}

		void setNumEntries(SerializedSubObject* obj, UINT32 numEntries)
		{
			obj->entries = UnorderedMap<UINT32, SerializedEntry>();
		}
	public:
		SerializedSubObjectRTTI()
		{
			addPlainField("typeId", 0, &SerializedSubObjectRTTI::getTypeId, &SerializedSubObjectRTTI::setTypeId);
			addReflectableArrayField("entries", 1, &SerializedSubObjectRTTI::getEntry, &SerializedSubObjectRTTI::getNumEntries,
				&SerializedSubObjectRTTI::setEntry, &SerializedSubObjectRTTI::setNumEntries);
		}

		virtual void onSerializationStarted(IReflectable* obj) override
		{
			SerializedSubObject* serializableObject = static_cast<SerializedSubObject*>(obj);

			Vector<SerializedEntry> sequentialData;
			for (auto& entry : serializableObject->entries)
				sequentialData.push_back(entry.second);

			serializableObject->mRTTIData = sequentialData;
		}

		virtual void onSerializationEnded(IReflectable* obj) override
		{
			SerializedSubObject* serializableObject = static_cast<SerializedSubObject*>(obj);
			serializableObject->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedSubObject";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedSubObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedSubObject>();
		}
	};

	class BS_UTILITY_EXPORT SerializedEntryRTTI : public RTTIType <SerializedEntry, IReflectable, SerializedEntryRTTI>
	{
	private:
		UINT32& getFieldId(SerializedEntry* obj)
		{
			return obj->fieldId;
		}

		void setFieldId(SerializedEntry* obj, UINT32& val)
		{
			obj->fieldId = val;
		}

		SPtr<SerializedInstance> getSerialized(SerializedEntry* obj)
		{
			return obj->serialized;
		}

		void setSerialized(SerializedEntry* obj, SPtr<SerializedInstance> val)
		{
			obj->serialized = val;
		}

	public:
		SerializedEntryRTTI()
		{
			addPlainField("fieldId", 0, &SerializedEntryRTTI::getFieldId, &SerializedEntryRTTI::setFieldId);
			addReflectablePtrField("serialized", 1, &SerializedEntryRTTI::getSerialized, &SerializedEntryRTTI::setSerialized);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedEntry";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedEntry;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedEntry>();
		}
	};

	class BS_UTILITY_EXPORT SerializedArrayEntryRTTI : public RTTIType <SerializedArrayEntry, IReflectable, SerializedArrayEntryRTTI>
	{
	private:
		UINT32& getArrayIdx(SerializedArrayEntry* obj)
		{
			return obj->index;
		}

		void setArrayIdx(SerializedArrayEntry* obj, UINT32& val)
		{
			obj->index = val;
		}

		SPtr<SerializedInstance> getSerialized(SerializedArrayEntry* obj)
		{
			return obj->serialized;
		}

		void setSerialized(SerializedArrayEntry* obj, SPtr<SerializedInstance> val)
		{
			obj->serialized = val;
		}

	public:
		SerializedArrayEntryRTTI()
		{
			addPlainField("index", 0, &SerializedArrayEntryRTTI::getArrayIdx, &SerializedArrayEntryRTTI::setArrayIdx);
			addReflectablePtrField("serialized", 1, &SerializedArrayEntryRTTI::getSerialized, &SerializedArrayEntryRTTI::setSerialized);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "SerializedArrayEntry";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_SerializedArrayEntry;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<SerializedArrayEntry>();
		}
	};

	/** @endcond */
}