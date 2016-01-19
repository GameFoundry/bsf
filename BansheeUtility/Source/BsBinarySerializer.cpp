#include "BsBinarySerializer.h"

#include "BsException.h"
#include "BsDebug.h"
#include "BsIReflectable.h"
#include "BsRTTIType.h"
#include "BsRTTIField.h"
#include "BsRTTIPlainField.h"
#include "BsRTTIReflectableField.h"
#include "BsRTTIReflectablePtrField.h"
#include "BsRTTIManagedDataBlockField.h"
#include "BsMemorySerializer.h"

#include <unordered_set>

/**
 * @brief	A macro that represents a block of code that gets used a lot inside
 * 			encodeInternal. It checks if the buffer has enough space, and if it does
 * 			it copies the data from the specified location and increments the needed
 * 			pointers and counters. If there is not enough space the buffer is flushed
 * 			(hopefully to make some space). If there is still not enough space the entire
 * 			encoding process ends.
 *
 * @param	dataPtr	Pointer to data which to copy.
 * @param	size   	Size of the data to copy
 */
#define COPY_TO_BUFFER(dataIter, size)									\
if((*bytesWritten + size##) > bufferLength)								\
{																		\
	mTotalBytesWritten += *bytesWritten;								\
	buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);	\
	if(buffer == nullptr || bufferLength < size##) return nullptr;		\
	*bytesWritten = 0;													\
}																		\
																		\
memcpy(buffer, dataIter##, size##);										\
buffer += size##;														\
*bytesWritten += size##;

namespace BansheeEngine
{
	BinarySerializer::BinarySerializer()
		:mLastUsedObjectId(1)
	{
	}

	void BinarySerializer::encode(IReflectable* object, UINT8* buffer, UINT32 bufferLength, 
		UINT32* bytesWritten, std::function<UINT8*(UINT8*, UINT32, UINT32&)> flushBufferCallback, bool shallow)
	{
		mObjectsToEncode.clear();
		mObjectAddrToId.clear();
		mLastUsedObjectId = 1;
		*bytesWritten = 0;
		mTotalBytesWritten = 0;
		UINT8* bufferStart = buffer;
		Vector<std::shared_ptr<IReflectable>> encodedObjects;

		UINT32 objectId = findOrCreatePersistentId(object);
		
		// Encode primary object and its value types
		buffer = encodeInternal(object, objectId, buffer, bufferLength, bytesWritten, flushBufferCallback, shallow);
		if(buffer == nullptr)
		{
			BS_EXCEPT(InternalErrorException, 
				"Destination buffer is null or not large enough.");
		}

		// Encode pointed to objects and their value types
		UnorderedSet<UINT32> serializedObjects;
		while(true)
		{
			auto iter = mObjectsToEncode.begin();
			bool foundObjectToProcess = false;
			for(iter; iter != mObjectsToEncode.end(); ++iter)
			{
				auto foundExisting = serializedObjects.find(iter->objectId);
				if(foundExisting != serializedObjects.end())
					continue; // Already processed

				std::shared_ptr<IReflectable> curObject = iter->object;
				UINT32 curObjectid = iter->objectId;
				serializedObjects.insert(curObjectid);
				mObjectsToEncode.erase(iter);

				buffer = encodeInternal(curObject.get(), curObjectid, buffer, 
					bufferLength, bytesWritten, flushBufferCallback, shallow);
				if(buffer == nullptr)
				{
					BS_EXCEPT(InternalErrorException, 
						"Destination buffer is null or not large enough.");
				}

				foundObjectToProcess = true;

				// Ensure we keep a reference to the object so it isn't released.
				// The system assigns unique IDs to IReflectable objects based on pointer 
				// addresses but if objects get released then same address could be assigned twice.
				// Note: To get around this I could assign unique IDs to IReflectable objects
				encodedObjects.push_back(curObject);

				break; // Need to start over as mObjectsToSerialize was possibly modified
			}

			if(!foundObjectToProcess) // We're done
				break;
		}

		// Final flush
		if(*bytesWritten > 0)
		{
			mTotalBytesWritten += *bytesWritten;
			buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
		}

		*bytesWritten = mTotalBytesWritten;

		encodedObjects.clear();
		mObjectsToEncode.clear();
		mObjectAddrToId.clear();
	}

	std::shared_ptr<IReflectable> BinarySerializer::decode(UINT8* data, UINT32 dataLength)
	{
		if (dataLength == 0)
			return nullptr;

		SPtr<SerializedObject> intermediateObject = _decodeIntermediate(data, dataLength);
		if (intermediateObject == nullptr)
			return nullptr;

		return _decodeIntermediate(intermediateObject);
	}

	SPtr<IReflectable> BinarySerializer::_decodeIntermediate(const SPtr<SerializedObject>& serializedObject)
	{
		mObjectMap.clear();

		SPtr<IReflectable> output;
		RTTITypeBase* type = IReflectable::_getRTTIfromTypeId(serializedObject->getRootTypeId());
		if (type != nullptr)
		{
			output = type->newRTTIObject();
			auto iterNewObj = mObjectMap.insert(std::make_pair(serializedObject, ObjectToDecode(output, serializedObject)));

			iterNewObj.first->second.decodeInProgress = true;
			decodeInternal(output, serializedObject);
			iterNewObj.first->second.decodeInProgress = false;
			iterNewObj.first->second.isDecoded = true;
		}

		// Go through the remaining objects (should be only ones with weak refs)
		for (auto iter = mObjectMap.begin(); iter != mObjectMap.end(); ++iter)
		{
			ObjectToDecode& objToDecode = iter->second;

			if (objToDecode.isDecoded)
				continue;

			objToDecode.decodeInProgress = true;
			decodeInternal(objToDecode.object, objToDecode.serializedObject);
			objToDecode.decodeInProgress = false;
			objToDecode.isDecoded = true;
		}

		mObjectMap.clear();
		return output;
	}

	UINT8* BinarySerializer::encodeInternal(IReflectable* object, UINT32 objectId, UINT8* buffer, UINT32& bufferLength, 
		UINT32* bytesWritten, std::function<UINT8*(UINT8*, UINT32, UINT32&)> flushBufferCallback, bool shallow)
	{
		RTTITypeBase* si = object->getRTTI();
		bool isBaseClass = false;

		// If an object has base classes, we need to iterate through all of them
		do
		{
			si->onSerializationStarted(object);

			// Encode object ID & type
			ObjectMetaData objectMetaData = encodeObjectMetaData(objectId, si->getRTTIId(), isBaseClass);
			COPY_TO_BUFFER(&objectMetaData, sizeof(ObjectMetaData))

			int numFields = si->getNumFields();
			for(int i = 0; i < numFields; i++)
			{
				RTTIField* curGenericField = si->getField(i);

				// Copy field ID & other meta-data like field size and type
				int metaData = encodeFieldMetaData(curGenericField->mUniqueId, curGenericField->getTypeSize(), 
					curGenericField->mIsVectorType, curGenericField->mType, curGenericField->hasDynamicSize(), false);
				COPY_TO_BUFFER(&metaData, META_SIZE)

				if(curGenericField->mIsVectorType)
				{
					UINT32 arrayNumElems = curGenericField->getArraySize(object);

					// Copy num vector elements
					COPY_TO_BUFFER(&arrayNumElems, NUM_ELEM_FIELD_SIZE)

					switch(curGenericField->mType)
					{
					case SerializableFT_ReflectablePtr:
						{
							RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

							for(UINT32 arrIdx = 0; arrIdx < arrayNumElems; arrIdx++)
							{
								std::shared_ptr<IReflectable> childObject;
								
								if (!shallow)
									childObject = curField->getArrayValue(object, arrIdx);

								UINT32 objId = registerObjectPtr(childObject);
								COPY_TO_BUFFER(&objId, sizeof(UINT32))
							}

							break;
						}
					case SerializableFT_Reflectable:
						{
							RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

							for(UINT32 arrIdx = 0; arrIdx < arrayNumElems; arrIdx++)
							{
								IReflectable& childObject = curField->getArrayValue(object, arrIdx);

								buffer = complexTypeToBuffer(&childObject, buffer, bufferLength, 
									bytesWritten, flushBufferCallback, shallow);
								if(buffer == nullptr)
								{
									si->onSerializationEnded(object);
									return nullptr;
								}
							}

							break;
						}
					case SerializableFT_Plain:
						{
							RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

							for(UINT32 arrIdx = 0; arrIdx < arrayNumElems; arrIdx++)
							{
								UINT32 typeSize = 0;
								if(curField->hasDynamicSize())
									typeSize = curField->getArrayElemDynamicSize(object, arrIdx);
								else
									typeSize = curField->getTypeSize();

								if ((*bytesWritten + typeSize) > bufferLength)
								{
									UINT8* tempBuffer = (UINT8*)bs_stack_alloc(typeSize);
									curField->arrayElemToBuffer(object, arrIdx, tempBuffer);

									buffer = dataBlockToBuffer(tempBuffer, typeSize, buffer, bufferLength, bytesWritten, flushBufferCallback);
									if (buffer == nullptr || bufferLength == 0)
									{
										bs_stack_free(tempBuffer);
										si->onSerializationEnded(object);
										return nullptr;
									}

									bs_stack_free(tempBuffer);
								}
								else
								{
									curField->arrayElemToBuffer(object, arrIdx, buffer);
									buffer += typeSize;
									*bytesWritten += typeSize;
								}
							}

							break;
						}
					default:
						BS_EXCEPT(InternalErrorException, 
							"Error encoding data. Encountered a type I don't know how to encode. Type: " + toString(UINT32(curGenericField->mType)) + 
							", Is array: " + toString(curGenericField->mIsVectorType));
					}
				}
				else
				{
					switch(curGenericField->mType)
					{
					case SerializableFT_ReflectablePtr:
						{
							RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);
							std::shared_ptr<IReflectable> childObject;
							
							if (!shallow)
								childObject = curField->getValue(object);

							UINT32 objId = registerObjectPtr(childObject);
							COPY_TO_BUFFER(&objId, sizeof(UINT32))

							break;
						}
					case SerializableFT_Reflectable:
						{
							RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);
							IReflectable& childObject = curField->getValue(object);

							buffer = complexTypeToBuffer(&childObject, buffer, bufferLength, 
								bytesWritten, flushBufferCallback, shallow);
							if(buffer == nullptr)
							{
								si->onSerializationEnded(object);
								return nullptr;
							}

							break;
						}
					case SerializableFT_Plain:
						{
							RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

							UINT32 typeSize = 0;
							if(curField->hasDynamicSize())
								typeSize = curField->getDynamicSize(object);
							else
								typeSize = curField->getTypeSize();

							if ((*bytesWritten + typeSize) > bufferLength)
							{
								UINT8* tempBuffer = (UINT8*)bs_stack_alloc(typeSize);
								curField->toBuffer(object, tempBuffer);
								
								buffer = dataBlockToBuffer(tempBuffer, typeSize, buffer, bufferLength, bytesWritten, flushBufferCallback);
								if (buffer == nullptr || bufferLength == 0)
								{
									bs_stack_free(tempBuffer);
									si->onSerializationEnded(object);
									return nullptr;
								}

								bs_stack_free(tempBuffer);
							}
							else
							{
								curField->toBuffer(object, buffer);
								buffer += typeSize;
								*bytesWritten += typeSize;
							}

							break;
						}
					case SerializableFT_DataBlock:
						{
							RTTIManagedDataBlockFieldBase* curField = static_cast<RTTIManagedDataBlockFieldBase*>(curGenericField);
							ManagedDataBlock value = curField->getValue(object);

							// Data block size
							UINT32 dataBlockSize = value.getSize();
							COPY_TO_BUFFER(&dataBlockSize, sizeof(UINT32))

							// Data block data
							UINT8* dataToStore = value.getData();

							buffer = dataBlockToBuffer(dataToStore, dataBlockSize, buffer, bufferLength, bytesWritten, flushBufferCallback);
							if (buffer == nullptr || bufferLength == 0)
							{
								si->onSerializationEnded(object);
								return nullptr;
							}

							break;
						}
					default:
						BS_EXCEPT(InternalErrorException, 
							"Error encoding data. Encountered a type I don't know how to encode. Type: " + toString(UINT32(curGenericField->mType)) + 
							", Is array: " + toString(curGenericField->mIsVectorType));
					}
				}
			}

			si->onSerializationEnded(object);

			si = si->getBaseClass();
			isBaseClass = true;

		} while(si != nullptr); // Repeat until we reach the top of the inheritance hierarchy

		return buffer;
	}

	SPtr<SerializedObject> BinarySerializer::_encodeIntermediate(IReflectable* object, bool shallow)
	{
		// TODO: This is a hacky way of generating an intermediate format to save development time and complexity.
		// It is hacky because it requires a full on encode to binary and then decode into intermediate. It should 
		// be better to modify encoding process so it outputs the intermediate format directly (similar to how decoding works). 
		// This also means that once you have an intermediate format you cannot use it to encode to binary. 

		std::function<void*(UINT32)> allocator = &MemoryAllocator<GenAlloc>::allocate;

		MemorySerializer ms;
		UINT32 dataLength = 0;
		UINT8* data = ms.encode(object, dataLength, allocator, shallow);

		BinarySerializer bs;
		SPtr<SerializedObject> obj = bs._decodeIntermediate(data, dataLength, true);

		bs_free(data);
		return obj;
	}

	SPtr<SerializedObject> BinarySerializer::_decodeIntermediate(UINT8* data, UINT32 dataLength, bool copyData)
	{
		UINT32 bytesRead = 0;
		mInterimObjectMap.clear();

		SPtr<SerializedObject> rootObj;
		bool hasMore = decodeIntermediateInternal(data, dataLength, bytesRead, rootObj, copyData);
		while (hasMore)
		{
			UINT8* dataPtr = data + bytesRead;

			SPtr<SerializedObject> dummyObj;
			hasMore = decodeIntermediateInternal(dataPtr, dataLength, bytesRead, dummyObj, copyData);
		}

		return rootObj;
	}

	bool BinarySerializer::decodeIntermediateInternal(UINT8* data, UINT32 dataLength, UINT32& bytesRead, SPtr<SerializedObject>& output, bool copyData)
	{
		if ((bytesRead + sizeof(ObjectMetaData)) > dataLength)
		{
			BS_EXCEPT(InternalErrorException,
				"Error decoding data.");
		}

		ObjectMetaData objectMetaData;
		objectMetaData.objectMeta = 0;
		objectMetaData.typeId = 0;
		memcpy(&objectMetaData, data, sizeof(ObjectMetaData));
		data += sizeof(ObjectMetaData);
		bytesRead += sizeof(ObjectMetaData);

		UINT32 objectId = 0;
		UINT32 objectTypeId = 0;
		bool objectIsBaseClass = false;
		decodeObjectMetaData(objectMetaData, objectId, objectTypeId, objectIsBaseClass);

		if (objectIsBaseClass)
		{
			BS_EXCEPT(InternalErrorException, "Encountered a base-class object while looking for a new object. " \
				"Base class objects are only supposed to be parts of a larger object.");
		}

		RTTITypeBase* rtti = IReflectable::_getRTTIfromTypeId(objectTypeId);
		SerializedSubObject* serializedSubObject = nullptr;
		
		if (rtti != nullptr)
		{
			if (objectId > 0)
			{
				auto iterFind = mInterimObjectMap.find(objectId);
				if (iterFind == mInterimObjectMap.end())
				{
					output = bs_shared_ptr_new<SerializedObject>();
					mInterimObjectMap.insert(std::make_pair(objectId, output));
				}
				else
					output = iterFind->second;
			}
			else // Not a reflectable ptr referenced object
				output = bs_shared_ptr_new<SerializedObject>();

			output->subObjects.push_back(SerializedSubObject());
			serializedSubObject = &output->subObjects.back();

			serializedSubObject->typeId = objectTypeId;
		}

		while (bytesRead < dataLength)
		{
			int metaData = -1;

			if ((bytesRead + META_SIZE) > dataLength)
			{
				BS_EXCEPT(InternalErrorException,
					"Error decoding data.");
			}

			memcpy((void*)&metaData, data, META_SIZE);

			if (isObjectMetaData(metaData)) // We've reached a new object or a base class of the current one
			{
				if ((bytesRead + sizeof(ObjectMetaData)) > dataLength)
				{
					BS_EXCEPT(InternalErrorException,
						"Error decoding data.");
				}

				ObjectMetaData objMetaData;
				objMetaData.objectMeta = 0;
				objMetaData.typeId = 0;
				memcpy(&objMetaData, data, sizeof(ObjectMetaData));

				UINT32 objId = 0;
				UINT32 objTypeId = 0;
				bool objIsBaseClass = false;
				decodeObjectMetaData(objMetaData, objId, objTypeId, objIsBaseClass);

				// If it's a base class, get base class RTTI and handle that
				if (objIsBaseClass)
				{
					if (rtti != nullptr)
						rtti = rtti->getBaseClass();

					// Saved and current base classes don't match, so just skip over all that data
					if (rtti == nullptr || rtti->getRTTIId() != objTypeId)
					{
						rtti = nullptr;
					}

					if (rtti != nullptr)
					{
						output->subObjects.push_back(SerializedSubObject());
						serializedSubObject = &output->subObjects.back();

						serializedSubObject->typeId = objTypeId;
					}

					data += sizeof(ObjectMetaData);
					bytesRead += sizeof(ObjectMetaData);
					continue;
				}
				else
				{
					// Found new object, we're done
					return true;
				}
			}

			data += META_SIZE;
			bytesRead += META_SIZE;

			bool isArray;
			SerializableFieldType fieldType;
			UINT16 fieldId;
			UINT8 fieldSize;
			bool hasDynamicSize;
			bool terminator;
			decodeFieldMetaData(metaData, fieldId, fieldSize, isArray, fieldType, hasDynamicSize, terminator);

			if (terminator)
			{
				// We've processed the last field in this object, so return. Although we return false we don't actually know
				// if there is an object following this one. However it doesn't matter since terminator fields are only used 
				// for embedded objects that are all processed within this method so we can compensate.
				return false;
			}

			RTTIField* curGenericField = nullptr;

			if (rtti != nullptr)
				curGenericField = rtti->findField(fieldId);

			if (curGenericField != nullptr)
			{
				if (!hasDynamicSize && curGenericField->getTypeSize() != fieldSize)
				{
					BS_EXCEPT(InternalErrorException,
						"Data type mismatch. Type size stored in file and actual type size don't match. ("
						+ toString(curGenericField->getTypeSize()) + " vs. " + toString(fieldSize) + ")");
				}

				if (curGenericField->mIsVectorType != isArray)
				{
					BS_EXCEPT(InternalErrorException,
						"Data type mismatch. One is array, other is a single type.");
				}

				if (curGenericField->mType != fieldType)
				{
					BS_EXCEPT(InternalErrorException,
						"Data type mismatch. Field types don't match. " + toString(UINT32(curGenericField->mType)) + " vs. " + toString(UINT32(fieldType)));
				}
			}

			SPtr<SerializedInstance> serializedEntry;
			bool hasModification = false;

			int arrayNumElems = 1;
			if (isArray)
			{
				if ((bytesRead + NUM_ELEM_FIELD_SIZE) > dataLength)
				{
					BS_EXCEPT(InternalErrorException,
						"Error decoding data.");
				}

				memcpy((void*)&arrayNumElems, data, NUM_ELEM_FIELD_SIZE);
				data += NUM_ELEM_FIELD_SIZE;
				bytesRead += NUM_ELEM_FIELD_SIZE;

				SPtr<SerializedArray> serializedArray;
				if (curGenericField != nullptr)
				{
					serializedArray = bs_shared_ptr_new<SerializedArray>();
					serializedArray->numElements = arrayNumElems;

					serializedEntry = serializedArray;
					hasModification = true;
				}

				switch (fieldType)
				{
				case SerializableFT_ReflectablePtr:
				{
					RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

					for (int i = 0; i < arrayNumElems; i++)
					{
						if ((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
						{
							BS_EXCEPT(InternalErrorException,
								"Error decoding data.");
						}

						int childObjectId = 0;
						memcpy(&childObjectId, data, COMPLEX_TYPE_FIELD_SIZE);
						data += COMPLEX_TYPE_FIELD_SIZE;
						bytesRead += COMPLEX_TYPE_FIELD_SIZE;

						if (curField != nullptr)
						{
							SPtr<SerializedObject> serializedArrayEntry = nullptr;
							
							if (childObjectId > 0)
							{
								auto findObj = mInterimObjectMap.find(childObjectId);
								if (findObj == mInterimObjectMap.end())
								{
									serializedArrayEntry = bs_shared_ptr_new<SerializedObject>();
									mInterimObjectMap.insert(std::make_pair(childObjectId, serializedArrayEntry));
								}
								else
									serializedArrayEntry = findObj->second;
							}

							SerializedArrayEntry arrayEntry;
							arrayEntry.serialized = serializedArrayEntry;
							arrayEntry.index = i;

							serializedArray->entries[i] = arrayEntry;
						}
					}

					break;
				}
				case SerializableFT_Reflectable:
				{
					RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

					for (int i = 0; i < arrayNumElems; i++)
					{
						if (curField != nullptr)
						{
							UINT32 bytesReadStart = bytesRead;
							SPtr<SerializedObject> serializedArrayEntry;
							decodeIntermediateInternal(data, dataLength, bytesRead, serializedArrayEntry, copyData);

							SerializedArrayEntry arrayEntry;
							arrayEntry.serialized = serializedArrayEntry;
							arrayEntry.index = i;

							serializedArray->entries[i] = arrayEntry;

							UINT32 complexTypeSize = bytesRead - bytesReadStart;
							data += complexTypeSize;
						}
					}
					break;
				}
				case SerializableFT_Plain:
				{
					RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

					for (int i = 0; i < arrayNumElems; i++)
					{
						UINT32 typeSize = fieldSize;
						if (hasDynamicSize)
							memcpy(&typeSize, data, sizeof(UINT32));

						if (curField != nullptr)
						{
							SPtr<SerializedField> serializedField = bs_shared_ptr_new<SerializedField>();

							if (copyData)
							{
								serializedField->value = (UINT8*)bs_alloc(typeSize);
								memcpy(serializedField->value, data, typeSize);
								serializedField->ownsMemory = true;
							}
							else
								serializedField->value = data;

							serializedField->size = typeSize;

							SerializedArrayEntry arrayEntry;
							arrayEntry.serialized = serializedField;
							arrayEntry.index = i;

							serializedArray->entries[i] = arrayEntry;
						}

						data += typeSize;
						bytesRead += typeSize;
					}
					break;
				}
				default:
					BS_EXCEPT(InternalErrorException,
						"Error decoding data. Encountered a type I don't know how to decode. Type: " + toString(UINT32(fieldType)) +
						", Is array: " + toString(isArray));
				}
			}
			else
			{
				switch (fieldType)
				{
				case SerializableFT_ReflectablePtr:
				{
					RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

					if ((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
					{
						BS_EXCEPT(InternalErrorException,
							"Error decoding data.");
					}

					int childObjectId = 0;
					memcpy(&childObjectId, data, COMPLEX_TYPE_FIELD_SIZE);
					data += COMPLEX_TYPE_FIELD_SIZE;
					bytesRead += COMPLEX_TYPE_FIELD_SIZE;

					if (curField != nullptr)
					{
						SPtr<SerializedObject> serializedField = nullptr;

						if (childObjectId > 0)
						{
							auto findObj = mInterimObjectMap.find(childObjectId);
							if (findObj == mInterimObjectMap.end())
							{
								serializedField = bs_shared_ptr_new<SerializedObject>();
								mInterimObjectMap.insert(std::make_pair(childObjectId, serializedField));
							}
							else
								serializedField = findObj->second;
						}

						serializedEntry = serializedField;
						hasModification = true;
					}

					break;
				}
				case SerializableFT_Reflectable:
				{
					RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

					if (curField != nullptr)
					{
						UINT32 bytesReadStart = bytesRead;
						SPtr<SerializedObject> serializedChildObj;
						decodeIntermediateInternal(data, dataLength, bytesRead, serializedChildObj, copyData);

						serializedEntry = serializedChildObj;
						hasModification = true;

						UINT32 complexTypeSize = bytesRead - bytesReadStart;
						data += complexTypeSize;
					}

					break;
				}
				case SerializableFT_Plain:
				{
					RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

					UINT32 typeSize = fieldSize;
					if (hasDynamicSize)
						memcpy(&typeSize, data, sizeof(UINT32));

					if (curField != nullptr)
					{
						SPtr<SerializedField> serializedField = bs_shared_ptr_new<SerializedField>();

						if (copyData)
						{
							serializedField->value = (UINT8*)bs_alloc(typeSize);
							memcpy(serializedField->value, data, typeSize);
							serializedField->ownsMemory = true;
						}
						else
							serializedField->value = data;

						serializedField->size = typeSize;

						serializedEntry = serializedField;
						hasModification = true;
					}

					data += typeSize;
					bytesRead += typeSize;
					break;
				}
				case SerializableFT_DataBlock:
				{
					RTTIManagedDataBlockFieldBase* curField = static_cast<RTTIManagedDataBlockFieldBase*>(curGenericField);

					if ((bytesRead + DATA_BLOCK_TYPE_FIELD_SIZE) > dataLength)
					{
						BS_EXCEPT(InternalErrorException,
							"Error decoding data.");
					}

					// Data block size
					UINT32 dataBlockSize = 0;
					memcpy(&dataBlockSize, data, DATA_BLOCK_TYPE_FIELD_SIZE);
					data += DATA_BLOCK_TYPE_FIELD_SIZE;
					bytesRead += DATA_BLOCK_TYPE_FIELD_SIZE;

					if ((bytesRead + dataBlockSize) > dataLength)
					{
						BS_EXCEPT(InternalErrorException,
							"Error decoding data.");
					}

					// Data block data
					if (curField != nullptr)
					{
						SPtr<SerializedField> serializedField = bs_shared_ptr_new<SerializedField>();

						if (copyData)
						{
							serializedField->value = (UINT8*)bs_alloc(dataBlockSize);
							memcpy(serializedField->value, data, dataBlockSize);
							serializedField->ownsMemory = true;
						}
						else
							serializedField->value = data;

						serializedField->size = dataBlockSize;

						serializedEntry = serializedField;
						hasModification = true;
					}

					data += dataBlockSize;
					bytesRead += dataBlockSize;

					break;
				}
				default:
					BS_EXCEPT(InternalErrorException,
						"Error decoding data. Encountered a type I don't know how to decode. Type: " + toString(UINT32(fieldType)) +
						", Is array: " + toString(isArray));
				}
			}

			if (hasModification)
			{
				SerializedEntry entry;
				entry.fieldId = curGenericField->mUniqueId;
				entry.serialized = serializedEntry;

				serializedSubObject->entries.insert(std::make_pair(curGenericField->mUniqueId, entry));
			}
		}

		return false;
	}

	void BinarySerializer::decodeInternal(const SPtr<IReflectable>& object, const SPtr<SerializedObject>& serializableObject)
	{
		UINT32 numSubObjects = (UINT32)serializableObject->subObjects.size();

		Vector<RTTITypeBase*> rttiTypes;
		for (UINT32 subObjectIdx = 0; subObjectIdx < numSubObjects; subObjectIdx++)
		{
			const SerializedSubObject& subObject = serializableObject->subObjects[subObjectIdx];

			RTTITypeBase* rtti = IReflectable::_getRTTIfromTypeId(subObject.typeId);
			if (rtti == nullptr)
				continue;

			rtti->onDeserializationStarted(object.get());
			rttiTypes.push_back(rtti);

			UINT32 numFields = rtti->getNumFields();
			for (UINT32 fieldIdx = 0; fieldIdx < numFields; fieldIdx++)
			{
				RTTIField* curGenericField = rtti->getField(fieldIdx);

				auto iterFindFieldData = subObject.entries.find(curGenericField->mUniqueId);
				if (iterFindFieldData == subObject.entries.end())
					continue;

				SPtr<SerializedInstance> entryData = iterFindFieldData->second.serialized;
				if (curGenericField->isArray())
				{
					SPtr<SerializedArray> arrayData = std::static_pointer_cast<SerializedArray>(entryData);

					UINT32 arrayNumElems = (UINT32)arrayData->numElements;
					curGenericField->setArraySize(object.get(), arrayNumElems);

					switch (curGenericField->mType)
					{
					case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

						for (auto& arrayElem : arrayData->entries)
						{
							SPtr<SerializedObject> arrayElemData = std::static_pointer_cast<SerializedObject>(arrayElem.second.serialized);
							RTTITypeBase* childRtti = nullptr;
							
							if (arrayElemData != nullptr)
								childRtti = IReflectable::_getRTTIfromTypeId(arrayElemData->getRootTypeId());

							if (childRtti != nullptr)
							{
								auto findObj = mObjectMap.find(arrayElemData);
								if (findObj == mObjectMap.end())
								{
									SPtr<IReflectable> newObject = childRtti->newRTTIObject();
									findObj = mObjectMap.insert(std::make_pair(arrayElemData, ObjectToDecode(newObject, arrayElemData))).first;
								}

								ObjectToDecode& objToDecode = findObj->second;

								bool needsDecoding = (curField->getFlags() & RTTI_Flag_WeakRef) == 0 && !objToDecode.isDecoded;
								if (needsDecoding)
								{
									if (objToDecode.decodeInProgress)
									{
										LOGWRN("Detected a circular reference when decoding. Referenced object fields " \
											"will be resolved in an undefined order (i.e. one of the objects will not " \
											"be fully deserialized when assigned to its field). Use RTTI_Flag_WeakRef to " \
											"get rid of this warning and tell the system which of the objects is allowed " \
											"to be deserialized after it is assigned to its field.");
									}
									else
									{
										objToDecode.decodeInProgress = true;
										decodeInternal(objToDecode.object, objToDecode.serializedObject);
										objToDecode.decodeInProgress = false;
										objToDecode.isDecoded = true;
									}
								}

								curField->setArrayValue(object.get(), arrayElem.first, objToDecode.object);
							}
							else
							{
								curField->setArrayValue(object.get(), arrayElem.first, nullptr);
							}
						}
					}
						break;
					case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

						for (auto& arrayElem : arrayData->entries)
						{
							SPtr<SerializedObject> arrayElemData = std::static_pointer_cast<SerializedObject>(arrayElem.second.serialized);
							RTTITypeBase* childRtti = nullptr;

							if (arrayElemData != nullptr)
								childRtti = IReflectable::_getRTTIfromTypeId(arrayElemData->getRootTypeId());

							if (childRtti != nullptr)
							{
								SPtr<IReflectable> newObject = childRtti->newRTTIObject();
								decodeInternal(newObject, arrayElemData);
								curField->setArrayValue(object.get(), arrayElem.first, *newObject);
							}
						}
						break;
					}
					case SerializableFT_Plain:
					{
						RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

						for (auto& arrayElem : arrayData->entries)
						{
							SPtr<SerializedField> fieldData = std::static_pointer_cast<SerializedField>(arrayElem.second.serialized);
							if (fieldData != nullptr)
							{
								curField->arrayElemFromBuffer(object.get(), arrayElem.first, fieldData->value);
							}
						}
					}
						break;
					}
				}
				else
				{
					switch (curGenericField->mType)
					{
					case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

						SPtr<SerializedObject> fieldObjectData = std::static_pointer_cast<SerializedObject>(entryData);
						RTTITypeBase* childRtti = nullptr;

						if (fieldObjectData != nullptr)
							childRtti = IReflectable::_getRTTIfromTypeId(fieldObjectData->getRootTypeId());

						if (childRtti != nullptr)
						{
							auto findObj = mObjectMap.find(fieldObjectData);
							if (findObj == mObjectMap.end())
							{
								SPtr<IReflectable> newObject = childRtti->newRTTIObject();
								findObj = mObjectMap.insert(std::make_pair(fieldObjectData, ObjectToDecode(newObject, fieldObjectData))).first;
							}

							ObjectToDecode& objToDecode = findObj->second;

							bool needsDecoding = (curField->getFlags() & RTTI_Flag_WeakRef) == 0 && !objToDecode.isDecoded;
							if (needsDecoding)
							{
								if (objToDecode.decodeInProgress)
								{
									LOGWRN("Detected a circular reference when decoding. Referenced object's fields " \
										"will be resolved in an undefined order (i.e. one of the objects will not " \
										"be fully deserialized when assigned to its field). Use RTTI_Flag_WeakRef to " \
										"get rid of this warning and tell the system which of the objects is allowed " \
										"to be deserialized after it is assigned to its field.");
								}
								else
								{
									objToDecode.decodeInProgress = true;
									decodeInternal(objToDecode.object, objToDecode.serializedObject);
									objToDecode.decodeInProgress = false;
									objToDecode.isDecoded = true;
								}
							}

							curField->setValue(object.get(), objToDecode.object);
						}
						else
						{
							curField->setValue(object.get(), nullptr);
						}
					}
						break;
					case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

						SPtr<SerializedObject> fieldObjectData = std::static_pointer_cast<SerializedObject>(entryData);
						RTTITypeBase* childRtti = nullptr;

						if (fieldObjectData != nullptr)
							childRtti = IReflectable::_getRTTIfromTypeId(fieldObjectData->getRootTypeId());

						if (childRtti != nullptr)
						{
							SPtr<IReflectable> newObject = childRtti->newRTTIObject();
							decodeInternal(newObject, fieldObjectData);
							curField->setValue(object.get(), *newObject);
						}
						break;
					}
					case SerializableFT_Plain:
					{
						RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

						SPtr<SerializedField> fieldData = std::static_pointer_cast<SerializedField>(entryData);
						if (fieldData != nullptr)
						{
							curField->fromBuffer(object.get(), fieldData->value);
						}
					}
						break;
					case SerializableFT_DataBlock:
					{
						RTTIManagedDataBlockFieldBase* curField = static_cast<RTTIManagedDataBlockFieldBase*>(curGenericField);

						SPtr<SerializedField> fieldData = std::static_pointer_cast<SerializedField>(entryData);
						if (fieldData != nullptr)
						{
							UINT8* dataCopy = curField->allocate(object.get(), fieldData->size); // TODO - Low priority. I need to read files better, so I
							memcpy(dataCopy, fieldData->value, fieldData->size);		//    can just pass the buffer pointer directly without copying (possibly large amounts of data)

							ManagedDataBlock value(dataCopy, fieldData->size); // Not managed because I assume the owner class will decide whether to delete the data or keep it
							curField->setValue(object.get(), value);
						}

						break;
					}
					}
				}
			}
		}

		for (auto iterFind = rttiTypes.rbegin(); iterFind != rttiTypes.rend(); ++iterFind)
		{
			(*iterFind)->onDeserializationEnded(object.get());
		}
	}

	UINT32 BinarySerializer::encodeFieldMetaData(UINT16 id, UINT8 size, bool array, 
		SerializableFieldType type, bool hasDynamicSize, bool terminator)
	{
		// If O == 0 - Meta contains field information (Encoded using this method)
		//// Encoding: IIII IIII IIII IIII SSSS SSSS xTYP DCAO
		//// I - Id
		//// S - Size
		//// C - Complex
		//// A - Array
		//// D - Data block
		//// P - Complex ptr
		//// O - Object descriptor
		//// Y - Plain field has dynamic size
		//// T - Terminator (last field in an object)

		return (id << 16 | size << 8 | 
			(array ? 0x02 : 0) | 
			((type == SerializableFT_DataBlock) ? 0x04 : 0) | 
			((type == SerializableFT_Reflectable) ? 0x08 : 0) | 
			((type == SerializableFT_ReflectablePtr) ? 0x10 : 0) | 
			(hasDynamicSize ? 0x20 : 0) |
			(terminator ? 0x40 : 0)); // TODO - Low priority. Technically I could encode this much more tightly, and use var-ints for ID
	}

	void BinarySerializer::decodeFieldMetaData(UINT32 encodedData, UINT16& id, UINT8& size, 
		bool& array, SerializableFieldType& type, bool& hasDynamicSize, bool& terminator)
	{
		if(isObjectMetaData(encodedData))
		{
			BS_EXCEPT(InternalErrorException, 
				"Meta data represents an object description but is trying to be decoded as a field descriptor.");
		}

		terminator = (encodedData & 0x40) != 0;
		hasDynamicSize = (encodedData & 0x20) != 0;

		if((encodedData & 0x10) != 0)
			type = SerializableFT_ReflectablePtr;
		else if((encodedData & 0x08) != 0)
			type = SerializableFT_Reflectable;
		else if((encodedData & 0x04) != 0)
			type = SerializableFT_DataBlock;
		else
			type = SerializableFT_Plain;

		array = (encodedData & 0x02) != 0;
		size = (UINT8)((encodedData >> 8) & 0xFF);
		id = (UINT16)((encodedData >> 16) & 0xFFFF);
	}

	BinarySerializer::ObjectMetaData BinarySerializer::encodeObjectMetaData(UINT32 objId, UINT32 objTypeId, bool isBaseClass)
	{
		// If O == 1 - Meta contains object instance information (Encoded using encodeObjectMetaData)
		//// Encoding: SSSS SSSS SSSS SSSS xxxx xxxx xxxx xxBO
		//// S - Size of the object identifier
		//// O - Object descriptor
		//// B - Base class indicator
		
		if(objId > 1073741823)
		{
			BS_EXCEPT(InvalidParametersException, "Object ID is larger than we can store (max 30 bits): " + toString(objId));
		}

		ObjectMetaData metaData;
		metaData.objectMeta = (objId << 2) | (isBaseClass ? 0x02 : 0) | 0x01;
		metaData.typeId = objTypeId;
		return metaData;
	}

	void BinarySerializer::decodeObjectMetaData(BinarySerializer::ObjectMetaData encodedData, UINT32& objId, UINT32& objTypeId, bool& isBaseClass)
	{
		if(!isObjectMetaData(encodedData.objectMeta))
		{
			BS_EXCEPT(InternalErrorException, 
				"Meta data represents a field description but is trying to be decoded as an object descriptor.");
		}

		objId = (encodedData.objectMeta >> 2) & 0x3FFFFFFF;
		isBaseClass = (encodedData.objectMeta & 0x02) != 0;
		objTypeId = encodedData.typeId;
	}

	bool BinarySerializer::isObjectMetaData(UINT32 encodedData)
	{
		return ((encodedData & 0x01) != 0);
	}

	UINT8* BinarySerializer::complexTypeToBuffer(IReflectable* object, UINT8* buffer, UINT32& bufferLength, 
		UINT32* bytesWritten, std::function<UINT8*(UINT8*, UINT32, UINT32&)> flushBufferCallback, bool shallow)
	{
		if (object != nullptr)
		{
			buffer = encodeInternal(object, 0, buffer, bufferLength, bytesWritten, flushBufferCallback, shallow);

			// Encode terminator field
			// Complex types require terminator fields because they can be embedded within other complex types and we need
			// to know when their fields end and parent's resume
			int metaData = encodeFieldMetaData(0, 0, false, SerializableFT_Plain, false, true);
			COPY_TO_BUFFER(&metaData, META_SIZE)
		}

		return buffer;
	}

	UINT8* BinarySerializer::dataBlockToBuffer(UINT8* data, UINT32 size, UINT8* buffer, UINT32& bufferLength, UINT32* bytesWritten,
		std::function<UINT8*(UINT8* buffer, UINT32 bytesWritten, UINT32& newBufferSize)> flushBufferCallback)
	{
		UINT32 remainingSize = size;
		while (remainingSize > 0)
		{
			UINT32 remainingSpaceInBuffer = bufferLength - *bytesWritten;

			if (remainingSize <= remainingSpaceInBuffer)
			{
				COPY_TO_BUFFER(data, remainingSize);
				remainingSize = 0;
			}
			else
			{
				memcpy(buffer, data, remainingSpaceInBuffer);
				buffer += remainingSpaceInBuffer;
				*bytesWritten += remainingSpaceInBuffer;
				data += remainingSpaceInBuffer;
				remainingSize -= remainingSpaceInBuffer;

				mTotalBytesWritten += *bytesWritten;
				buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
				if (buffer == nullptr || bufferLength == 0)
					return nullptr;

				*bytesWritten = 0;
			}
		}

		return buffer;
	}

	UINT32 BinarySerializer::findOrCreatePersistentId(IReflectable* object)
	{
		void* ptrAddress = (void*)object;

		auto findIter = mObjectAddrToId.find(ptrAddress);
		if(findIter != mObjectAddrToId.end())
			return findIter->second;

		UINT32 objId = mLastUsedObjectId++;
		mObjectAddrToId.insert(std::make_pair(ptrAddress, objId));

		return objId;
	}

	UINT32 BinarySerializer::registerObjectPtr(std::shared_ptr<IReflectable> object)
	{
		if(object == nullptr)
			return 0;

		void* ptrAddress = (void*)object.get();

		auto iterFind = mObjectAddrToId.find(ptrAddress);
		if(iterFind == mObjectAddrToId.end())
		{
			UINT32 objId = findOrCreatePersistentId(object.get());

			mObjectsToEncode.push_back(ObjectToEncode(objId, object));
			mObjectAddrToId.insert(std::make_pair(ptrAddress, objId));

			return objId;
		}

		return iterFind->second;
	}
}

#undef COPY_TO_BUFFER