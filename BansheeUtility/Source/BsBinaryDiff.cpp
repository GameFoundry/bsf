#include "BsBinaryDiff.h"
#include "BsBinarySerializer.h"
#include "BsBinaryCloner.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	SPtr<SerializedObject> BinaryDiff::generateDiff(const SPtr<SerializedObject>& orgObj,
		const SPtr<SerializedObject>& newObj)
	{
		ObjectMap objectMap;
		return generateDiff(orgObj, newObj, objectMap);
	}

	SPtr<SerializedObject> BinaryDiff::generateDiff(const SPtr<SerializedObject>& orgObj, 
		const SPtr<SerializedObject>& newObj, ObjectMap& objectMap)
	{
		if (orgObj == nullptr || newObj == nullptr || orgObj->getRootTypeId() != newObj->getRootTypeId())
			return nullptr;

		RTTITypeBase* rootRtti = IReflectable::_getRTTIfromTypeId(newObj->getRootTypeId());
		if (rootRtti == nullptr)
			return nullptr;

		SPtr<SerializedObject> output;
		for (auto& subObject : newObj->subObjects)
		{
			RTTITypeBase* rtti = IReflectable::_getRTTIfromTypeId(subObject.typeId);
			if (rtti == nullptr)
				continue;

			SerializedSubObject* orgSubObject = nullptr;
			for (auto& curSubObject : orgObj->subObjects)
			{
				if (curSubObject.typeId == subObject.typeId)
				{
					orgSubObject = &curSubObject;
					break;
				}
			}
			
			SerializedSubObject* diffSubObject = nullptr;
			for (auto& newEntry : subObject.entries)
			{
				RTTIField* genericField = rtti->findField(newEntry.first);
				if (genericField == nullptr)
					continue;

				SPtr<SerializedInstance> newEntryData = newEntry.second.serialized;
				SPtr<SerializedInstance> orgEntryData;

				if (orgSubObject != nullptr)
				{
					auto orgEntryFind = orgSubObject->entries.find(newEntry.first);
					if (orgEntryFind != orgSubObject->entries.end())
						orgEntryData = newEntry.second.serialized;
				}

				SPtr<SerializedInstance> modification;
				bool hasModification = false;
				if (genericField->isArray())
				{
					SPtr<SerializedArray> orgArrayData = std::static_pointer_cast<SerializedArray>(orgEntryData);
					SPtr<SerializedArray> newArrayData = std::static_pointer_cast<SerializedArray>(newEntryData);

					SPtr<SerializedArray> serializedArray;

					if (newEntryData != nullptr && orgEntryData != nullptr)
					{
						for (auto& arrayEntryPair : newArrayData->entries)
						{
							SPtr<SerializedInstance> arrayModification;

							auto iterFind = orgArrayData->entries.find(arrayEntryPair.first);
							if (iterFind == orgArrayData->entries.end())
								arrayModification = arrayEntryPair.second.serialized->clone();
							else
							{
								arrayModification = generateDiff(genericField->mType, iterFind->second.serialized,
									arrayEntryPair.second.serialized, objectMap);
							}

							if (arrayModification != nullptr)
							{
								if (serializedArray == nullptr)
								{
									serializedArray = bs_shared_ptr<SerializedArray>();
									serializedArray->numElements = newArrayData->numElements;
								}

								SerializedArrayEntry arrayEntry;
								arrayEntry.index = arrayEntryPair.first;
								arrayEntry.serialized = arrayModification;
								serializedArray->entries[arrayEntryPair.first] = arrayEntry;
							}
						}
					}
					else if (newEntryData == nullptr)
					{
						serializedArray = bs_shared_ptr<SerializedArray>();
					}
					else if (orgEntryData == nullptr)
					{
						serializedArray = std::static_pointer_cast<SerializedArray>(newArrayData->clone());
					}

					modification = serializedArray;
					hasModification = modification != nullptr;
				}
				else
				{
					if (newEntryData != nullptr && orgEntryData != nullptr)
					{
						modification = generateDiff(genericField->mType, orgEntryData, newEntryData, objectMap);
						hasModification = modification != nullptr;
					}
					else if (newEntryData == nullptr)
					{
						switch (genericField->mType)
						{
						case SerializableFT_Plain:
						case SerializableFT_DataBlock:
							modification = bs_shared_ptr<SerializedField>();
							break;
						}

						hasModification = true;
					}
					else if (orgEntryData == nullptr)
					{
						modification = newEntryData->clone();
						hasModification = modification != nullptr;
					}
				}

				if (hasModification)
				{
					if (output == nullptr)
						output = bs_shared_ptr<SerializedObject>();

					if (diffSubObject == nullptr)
					{
						output->subObjects.push_back(SerializedSubObject());
						diffSubObject = &output->subObjects.back();
					}

					SerializedEntry modificationEntry;
					modificationEntry.fieldId = genericField->mUniqueId;
					modificationEntry.serialized = modification;
					diffSubObject->entries[genericField->mUniqueId] = modificationEntry;
				}
			}
		}

		return output;
	}

	SPtr<SerializedInstance> BinaryDiff::generateDiff(UINT32 fieldType, const SPtr<SerializedInstance>& orgData, 
		const SPtr<SerializedInstance>& newData, ObjectMap& objectMap)
	{
		SPtr<SerializedInstance> modification;
		switch (fieldType)
		{
		case SerializableFT_ReflectablePtr:
		case SerializableFT_Reflectable:
		{
			SPtr<SerializedObject> orgObjData = std::static_pointer_cast<SerializedObject>(orgData);
			SPtr<SerializedObject> newObjData = std::static_pointer_cast<SerializedObject>(newData);

			auto iterFind = objectMap.find(newObjData);
			if (iterFind != objectMap.end())
				modification = iterFind->second;
			else
			{
				SPtr<SerializedObject> objectDiff = generateDiff(orgObjData, newObjData);
				if (objectDiff != nullptr)
					objectMap[newObjData] = objectDiff;

				modification = objectDiff;
			}
		}
			break;
		case SerializableFT_Plain:
		case SerializableFT_DataBlock:
		{
			SPtr<SerializedField> orgFieldData = std::static_pointer_cast<SerializedField>(orgData);
			SPtr<SerializedField> newFieldData = std::static_pointer_cast<SerializedField>(newData);

			bool isModified = orgFieldData->size != newFieldData->size;
			if (!isModified)
				isModified = memcmp(orgFieldData->value, newFieldData->value, newFieldData->size) == 0;

			if (isModified)
				modification = newFieldData->clone();
		}
			break;
		}

		return modification;
	}

	void BinaryDiff::applyDiff(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& diff)
	{
		Vector<DiffCommand> commands;

		DiffObjectMap objectMap;
		applyDiff(object, diff, objectMap, commands);

		for (auto& command : commands)
		{
			bool isArray = (command.type & Diff_ArrayFlag) != 0;
			DiffCommandType type = (DiffCommandType)(command.type & 0xFF);
			IReflectable* destObject = command.object.get();

			switch (type)
			{
			case Diff_ArraySize: 
				command.field->setArraySize(destObject, command.size);
				break;
			case Diff_ObjectStart:
			{
				RTTITypeBase* curRtti = destObject->getRTTI();
				while (curRtti != nullptr)
				{
					curRtti->onDeserializationStarted(destObject);
					curRtti = curRtti->getBaseClass();
				}
			}
				break;
			case Diff_ObjectEnd: 
			{
				Stack<RTTITypeBase*> rttiTypes;
				RTTITypeBase* curRtti = destObject->getRTTI();
				while (curRtti != nullptr)
				{
					rttiTypes.push(curRtti);
					curRtti = curRtti->getBaseClass();
				}

				while (!rttiTypes.empty())
				{
					rttiTypes.top()->onDeserializationEnded(destObject);
					rttiTypes.pop();
				}
			}
				break;
			}

			if (isArray)
			{
				switch (type)
				{
				case Diff_ReflectablePtr:
				{
					RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(command.field);
					field->setArrayValue(destObject, command.arrayIdx, command.object);
				}
					break;
				case Diff_Reflectable: 
				{
					RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(command.field);
					field->setArrayValue(object.get(), command.arrayIdx, *command.object);
				}
					break;
				case Diff_Plain: 
				{
					RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(command.field);
					field->arrayElemFromBuffer(destObject, command.arrayIdx, command.value);
				}
					break;
				}
			}
			else
			{
				switch (type)
				{
				case Diff_ReflectablePtr:
				{
					RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(command.field);
					field->setValue(destObject, command.object);
				}
					break;
				case Diff_Reflectable:
				{
					RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(command.field);
					field->setValue(object.get(), *command.object);
				}
					break;
				case Diff_Plain:
				{
					RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(command.field);
					field->fromBuffer(destObject, command.value);
				}
					break;
				case Diff_DataBlock: 
				{
					RTTIManagedDataBlockFieldBase* field = static_cast<RTTIManagedDataBlockFieldBase*>(command.field);
					UINT8* dataCopy = field->allocate(destObject, command.size);
					memcpy(dataCopy, command.value, command.size);

					ManagedDataBlock value(dataCopy, command.size); // Not managed because I assume the owner class will decide whether to delete the data or keep it
					field->setValue(destObject, value);
				}
					break;
				}
			}
		}
	}

	void BinaryDiff::applyDiff(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& diff,
		DiffObjectMap& objectMap, Vector<DiffCommand>& diffCommands)
	{
		if (object == nullptr || diff == nullptr || object->getTypeId() != diff->getRootTypeId())
			return;

		DiffCommand objStartCommand;
		objStartCommand.field = nullptr;
		objStartCommand.type = Diff_ObjectStart;
		objStartCommand.object = object;

		diffCommands.push_back(objStartCommand);

		Stack<RTTITypeBase*> rttiTypes;
		for (auto& subObject : diff->subObjects)
		{
			for (auto& diffEntry : subObject.entries)
			{
				RTTITypeBase* rtti = IReflectable::_getRTTIfromTypeId(subObject.typeId);
				if (rtti == nullptr)
					continue;

				if (!object->isDerivedFrom(rtti))
					continue;

				rtti->onSerializationStarted(object.get());
				rttiTypes.push(rtti);

				RTTIField* genericField = rtti->findField(diffEntry.first);
				if (genericField == nullptr)
					continue;

				SPtr<SerializedInstance> diffData = diffEntry.second.serialized;

				if (genericField->isArray())
				{
					SPtr<SerializedArray> diffArray = std::static_pointer_cast<SerializedArray>(diffData);

					UINT32 numArrayElements = (UINT32)diffArray->numElements;

					DiffCommand arraySizeCommand;
					arraySizeCommand.field = genericField;
					arraySizeCommand.type = Diff_ArraySize | Diff_ArrayFlag;
					arraySizeCommand.arraySize = numArrayElements;

					diffCommands.push_back(arraySizeCommand);

					switch (genericField->mType)
					{
					case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);

						UINT32 orgArraySize = genericField->getArraySize(object.get());
						for (auto& arrayElem : diffArray->entries)
						{
							SPtr<SerializedObject> arrayElemData = std::static_pointer_cast<SerializedObject>(diffData);

							DiffCommand command;
							command.field = genericField;
							command.type = Diff_ReflectablePtr | Diff_ArrayFlag;
							command.arrayIdx = arrayElem.first;

							if (arrayElemData == nullptr)
							{
								command.object = nullptr;
								diffCommands.push_back(command);
							}
							else
							{
								bool needsNewObject = arrayElem.first >= orgArraySize;

								if (!needsNewObject)
								{
									SPtr<IReflectable> childObj = field->getArrayValue(object.get(), arrayElem.first);
									if (childObj != nullptr)
										applyDiff(childObj, arrayElemData);
									else
										needsNewObject = true;
								}

								if (needsNewObject)
								{
									RTTITypeBase* childRtti = IReflectable::_getRTTIfromTypeId(arrayElemData->getRootTypeId());
									if (childRtti != nullptr)
									{
										auto findObj = objectMap.find(arrayElemData);
										if (findObj == objectMap.end())
										{
											SPtr<IReflectable> newObject = childRtti->newRTTIObject();
											findObj = objectMap.insert(std::make_pair(arrayElemData, newObject)).first;
										}

										applyDiff(findObj->second, arrayElemData);
										command.object = findObj->second;
										diffCommands.push_back(command);
									}
									else
									{
										command.object = nullptr;
										diffCommands.push_back(command);
									}
								}
							}
						}
					}
						break;
					case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);

						UINT32 orgArraySize = genericField->getArraySize(object.get());

						Vector<SPtr<IReflectable>> newArrayElements(numArrayElements);
						UINT32 minArrayLength = std::min(orgArraySize, numArrayElements);
						for (UINT32 i = 0; i < minArrayLength; i++)
						{
							IReflectable& childObj = field->getArrayValue(object.get(), i);
							newArrayElements[i] = BinaryCloner::clone(&childObj, true);
						}

						for (auto& arrayElem : diffArray->entries)
						{
							SPtr<SerializedObject> arrayElemData = std::static_pointer_cast<SerializedObject>(arrayElem.second.serialized);

							if (arrayElem.first < orgArraySize)
							{
								applyDiff(newArrayElements[arrayElem.first], arrayElemData);
							}
							else
							{
								RTTITypeBase* childRtti = IReflectable::_getRTTIfromTypeId(arrayElemData->getRootTypeId());
								if (childRtti != nullptr)
								{
									SPtr<IReflectable> newObject = childRtti->newRTTIObject();
									applyDiff(newObject, arrayElemData);

									newArrayElements[arrayElem.first] = newObject;
								}
							}
						}

						for (UINT32 i = 0; i < numArrayElements; i++)
						{
							DiffCommand command;
							command.field = genericField;
							command.type = Diff_Reflectable | Diff_ArrayFlag;
							command.arrayIdx = i;
							command.object = newArrayElements[i];

							diffCommands.push_back(command);
						}
					}
						break;
					case SerializableFT_Plain:
					{
						for (auto& arrayElem : diffArray->entries)
						{
							SPtr<SerializedField> fieldData = std::static_pointer_cast<SerializedField>(arrayElem.second.serialized);
							if (fieldData != nullptr)
							{
								DiffCommand command;
								command.field = genericField;
								command.type = Diff_DataBlock | Diff_ArrayFlag;
								command.value = fieldData->value;
								command.size = fieldData->size;
								command.arrayIdx = arrayElem.first;

								diffCommands.push_back(command);
							}
						}
					}
						break;
					}
				}
				else
				{
					switch (genericField->mType)
					{
					case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
						SPtr<SerializedObject> fieldObjectData = std::static_pointer_cast<SerializedObject>(diffData);

						DiffCommand command;
						command.field = genericField;
						command.type = Diff_ReflectablePtr;

						if (fieldObjectData == nullptr)
							command.object = nullptr;
						else
						{
							SPtr<IReflectable> childObj = field->getValue(object.get());
							if (childObj == nullptr)
							{
								RTTITypeBase* childRtti = IReflectable::_getRTTIfromTypeId(fieldObjectData->getRootTypeId());
								if (childRtti != nullptr)
								{
									auto findObj = objectMap.find(fieldObjectData);
									if (findObj == objectMap.end())
									{
										SPtr<IReflectable> newObject = childRtti->newRTTIObject();
										findObj = objectMap.insert(std::make_pair(fieldObjectData, newObject)).first;
									}

									applyDiff(findObj->second, fieldObjectData);
									command.object = findObj->second;
								}
								else
								{
									command.object = nullptr;
								}
							}
							else
							{
								applyDiff(childObj, fieldObjectData);
							}
						}

						diffCommands.push_back(command);
					}
						break;
					case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
						SPtr<SerializedObject> fieldObjectData = std::static_pointer_cast<SerializedObject>(diffData);

						IReflectable& childObj = field->getValue(object.get());
						std::shared_ptr<IReflectable> clonedObj = BinaryCloner::clone(&childObj, true);

						applyDiff(clonedObj, fieldObjectData);

						DiffCommand command;
						command.field = genericField;
						command.type = Diff_Reflectable;
						command.object = clonedObj;

						diffCommands.push_back(command);
					}
						break;
					case SerializableFT_Plain:
					{
						SPtr<SerializedField> diffFieldData = std::static_pointer_cast<SerializedField>(diffData);

						if (diffFieldData->size > 0)
						{
							DiffCommand command;
							command.field = genericField;
							command.type = Diff_Plain;
							command.value = diffFieldData->value;
							command.size = diffFieldData->size;

							diffCommands.push_back(command);
						}
					}
						break;
					case SerializableFT_DataBlock:
					{
						SPtr<SerializedField> diffFieldData = std::static_pointer_cast<SerializedField>(diffData);

						DiffCommand command;
						command.field = genericField;
						command.type = Diff_DataBlock;
						command.value = diffFieldData->value;
						command.size = diffFieldData->size;

						diffCommands.push_back(command);
					}
						break;
					}
				}
			}
		}

		DiffCommand objEndCommand;
		objEndCommand.field = nullptr;
		objEndCommand.type = Diff_ObjectEnd;
		objEndCommand.object = object;

		diffCommands.push_back(objEndCommand);

		while (!rttiTypes.empty())
		{
			rttiTypes.top()->onSerializationEnded(object.get());
			rttiTypes.pop();
		}
	}
}