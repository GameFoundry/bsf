#include "BsSerializedObject.h"
#include "BsSerializedObjectRTTI.h"

namespace BansheeEngine
{
	SPtr<SerializedInstance> SerializedField::clone(bool cloneData)
	{
		SPtr<SerializedField> copy = bs_shared_ptr<SerializedField>();
		copy->size = size;

		if (cloneData)
		{
			copy->value = (UINT8*)bs_alloc(size);
			memcpy(copy->value, value, size);
			copy->ownsMemory = true;
		}
		else
		{
			copy->value = value;
			copy->ownsMemory = false;
		}

		return copy;
	}

	SPtr<SerializedInstance> SerializedObject::clone(bool cloneData)
	{
		SPtr<SerializedObject> copy = bs_shared_ptr<SerializedObject>();
		copy->typeId = typeId;

		for (auto& entryPair : entries)
		{
			SerializedEntry entry = entryPair.second;
			entry.serialized = entry.serialized->clone(cloneData);

			copy->entries[entryPair.first] = entry;
		}

		return copy;
	}

	SPtr<SerializedInstance> SerializedArray::clone(bool cloneData)
	{
		SPtr<SerializedArray> copy = bs_shared_ptr<SerializedArray>();
		copy->numElements = numElements;

		for (auto& entryPair : entries)
		{
			SerializedArrayEntry entry = entryPair.second;
			entry.serialized = entry.serialized->clone(cloneData);

			copy->entries[entryPair.first] = entry;
		}

		return copy;
	}

	RTTITypeBase* SerializedInstance::getRTTIStatic()
	{
		return SerializedInstanceRTTI::instance();
	}

	RTTITypeBase* SerializedInstance::getRTTI() const
	{
		return SerializedInstance::getRTTIStatic();
	}

	RTTITypeBase* SerializedField::getRTTIStatic()
	{
		return SerializedFieldRTTI::instance();
	}

	RTTITypeBase* SerializedField::getRTTI() const
	{
		return SerializedField::getRTTIStatic();
	}

	RTTITypeBase* SerializedObject::getRTTIStatic()
	{
		return SerializedObjectRTTI::instance();
	}

	RTTITypeBase* SerializedObject::getRTTI() const
	{
		return SerializedObject::getRTTIStatic();
	}

	RTTITypeBase* SerializedArray::getRTTIStatic()
	{
		return SerializedArrayRTTI::instance();
	}

	RTTITypeBase* SerializedArray::getRTTI() const
	{
		return SerializedArray::getRTTIStatic();
	}

	RTTITypeBase* SerializedEntry::getRTTIStatic()
	{
		return SerializedEntryRTTI::instance();
	}

	RTTITypeBase* SerializedEntry::getRTTI() const
	{
		return SerializedEntry::getRTTIStatic();
	}

	RTTITypeBase* SerializedArrayEntry::getRTTIStatic()
	{
		return SerializedArrayEntryRTTI::instance();
	}

	RTTITypeBase* SerializedArrayEntry::getRTTI() const
	{
		return SerializedArrayEntry::getRTTIStatic();
	}
}