#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsSerializedObject.h"

namespace BansheeEngine
{
	class BS_UTILITY_EXPORT BinaryDiff
	{
		typedef UnorderedMap<SPtr<SerializedObject>, SPtr<SerializedObject>> ObjectMap;
		typedef UnorderedMap<SPtr<SerializedObject>, SPtr<IReflectable>> DiffObjectMap;
	public:
		static SPtr<SerializedObject> generateDiff(const SPtr<SerializedObject>& orgObj, const SPtr<SerializedObject>& newObj);
		static void applyDiff(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& diff);

	private:
		enum DiffCommandType
		{
			Diff_Plain = 0x01,
			Diff_Reflectable = 0x02,
			Diff_ReflectablePtr = 0x03,
			Diff_DataBlock = 0x04,
			Diff_ArraySize = 0x05,
			Diff_ObjectStart = 0x06,
			Diff_ObjectEnd = 0x07,
			Diff_ArrayFlag = 0x10
		};

		struct DiffCommand
		{
			RTTIField* field;
			UINT32 type;
			SPtr<IReflectable> object;
			UINT8* value;
			UINT32 size;

			union
			{
				UINT32 arrayIdx;
				UINT32 arraySize;
			};
		};

		static SPtr<SerializedObject> generateDiff(const SPtr<SerializedObject>& orgObj, const SPtr<SerializedObject>& newObj, ObjectMap& objectMap);
		static SPtr<SerializedInstance> generateDiff(UINT32 fieldType, const SPtr<SerializedInstance>& orgData, 
			const SPtr<SerializedInstance>& newData, ObjectMap& objectMap);

		static void applyDiff(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& diff, DiffObjectMap& objectMap, Vector<DiffCommand>& diffCommands);
		static SPtr<IReflectable> clone(IReflectable* object);
	};
}