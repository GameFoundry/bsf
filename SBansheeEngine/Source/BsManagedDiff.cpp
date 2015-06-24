#include "BsManagedDiff.h"
#include "BsManagedSerializableDiff.h"
#include "BsBinarySerializer.h"
#include "BsMemorySerializer.h"
#include "BsManagedSerializableObject.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	SPtr<SerializedObject> ManagedDiff::generateDiff(const SPtr<SerializedObject>& orgSerzObj,
		const SPtr<SerializedObject>& newSerzObj, ObjectMap& objectMap)
	{
		BinarySerializer bs;

		SPtr<ManagedSerializableObject> orgObj = std::static_pointer_cast<ManagedSerializableObject>(bs._decodeIntermediate(orgSerzObj));
		SPtr<ManagedSerializableObject> newObj = std::static_pointer_cast<ManagedSerializableObject>(bs._decodeIntermediate(newSerzObj));

		ManagedSerializableDiffPtr diff = ManagedSerializableDiff::create(orgObj, newObj);
		if (diff == nullptr)
			return nullptr;

		MemorySerializer ms;

		UINT32 dataLength = 0;
		UINT8* diffData = ms.encode(diff.get(), dataLength, &bs_alloc);

		SPtr<SerializedObject> output = bs_shared_ptr<SerializedObject>();
		output->subObjects.push_back(SerializedSubObject());

		SerializedSubObject& subObject = output->subObjects.back();
		subObject.typeId = ManagedSerializableObject::getRTTIStatic()->getRTTIId();

		SPtr<SerializedField> field = bs_shared_ptr<SerializedField>();
		field->value = diffData;
		field->size = dataLength;
		field->ownsMemory = true;

		SerializedEntry entry;
		entry.fieldId = 0;
		entry.serialized = field;

		subObject.entries[0] = entry;

		return output;
	}

	void ManagedDiff::applyDiff(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& serzDiff,
		DiffObjectMap& objectMap, Vector<DiffCommand>& diffCommands)
	{
		SPtr<SerializedField> field = std::static_pointer_cast<SerializedField>(serzDiff->subObjects[0].entries[0].serialized);

		MemorySerializer ms;
		ManagedSerializableDiffPtr diff = std::static_pointer_cast<ManagedSerializableDiff>(ms.decode(field->value, field->size));

		SPtr<ManagedSerializableObject> managedObj = std::static_pointer_cast<ManagedSerializableObject>(object);
		diff->apply(managedObj);
	}
}