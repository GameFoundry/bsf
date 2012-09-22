#include "CmBinarySerializer.h"

#include "CmException.h"
#include "CmIReflectable.h"
#include "CmRTTIType.h"
#include "CmRTTIField.h"
#include "CmRTTIPlainField.h"
#include "CmRTTIReflectableField.h"
#include "CmRTTIReflectablePtrField.h"
#include "CmRTTIManagedDataBlockField.h"

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
#define COPY_TO_BUFFER(dataPtr, size)									\
if((*bytesWritten + size##) > bufferLength)								\
{																		\
	mTotalBytesWritten += *bytesWritten;								\
	buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);	\
	if(buffer == nullptr || bufferLength < size##) return nullptr;		\
	*bytesWritten = 0;													\
}																		\
																		\
memcpy(buffer, dataPtr##, size##);										\
buffer += size##;														\
*bytesWritten += size##;

namespace CamelotEngine
{
	BinarySerializer::BinarySerializer()
		:mLastUsedObjectId(1)
	{
	}

	void BinarySerializer::encode(IReflectable* object, UINT8* buffer, UINT32 bufferLength, int* bytesWritten, boost::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
	{
		mObjectsToEncode.clear();
		mObjectAddrToId.clear();
		mLastUsedObjectId = 1;
		*bytesWritten = 0;
		mTotalBytesWritten = 0;
		UINT8* bufferStart = buffer;

		UINT32 objectId = findOrCreatePersistentId(object);
		
		// Encode primary object and its value types
		buffer = encodeInternal(object, objectId, buffer, bufferLength, bytesWritten, flushBufferCallback);
		if(buffer == nullptr)
		{
			CM_EXCEPT(InternalErrorException, 
				"Destination buffer is null or not large enough.");
		}

		// Encode pointed to objects and their value types
		std::unordered_set<UINT32> serializedObjects;
		while(true)
		{
			auto iter = mObjectsToEncode.begin();
			bool foundObjectToProcess = false;
			for(iter; iter != mObjectsToEncode.end(); ++iter)
			{
				auto foundExisting = serializedObjects.find(iter->objectId);
				if(foundExisting != serializedObjects.end())
					continue; // Already processed

				IReflectable* curObject = iter->object;
				buffer = encodeInternal(curObject, iter->objectId, buffer, bufferLength, bytesWritten, flushBufferCallback);
				if(buffer == nullptr)
				{
					CM_EXCEPT(InternalErrorException, 
						"Destination buffer is null or not large enough.");
				}

				serializedObjects.insert(iter->objectId);
				mObjectsToEncode.erase(iter);
				foundObjectToProcess = true;
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
	}

	void BinarySerializer::decode(IReflectable* object, UINT8* data, UINT32 dataLength)
	{
		mPtrsToResolve.clear();
		mDecodedObjects.clear();

		// Create initial object + all other objects that are being referenced.
		// Use fields to find the type of referenced object.
		UINT32 bytesRead = 0;
		while(decodeInternal(object, data, dataLength, bytesRead))
		{
			data += bytesRead;

			UINT8* localDataPtr = data;
			UINT32 localBytesRead = bytesRead;

			if((bytesRead + sizeof(UINT32)) > dataLength)
			{
				CM_EXCEPT(InternalErrorException, 
					"Error decoding data.");
			}

			UINT32 objectMetaData = 0;
			memcpy(&objectMetaData, localDataPtr, sizeof(UINT32));
			localDataPtr += sizeof(UINT32);
			localBytesRead += sizeof(UINT32);

			UINT32 objectId = 0;
			decodeObjectMetaData(objectMetaData, objectId);

			object = nullptr;
			if(objectId != 0)
			{
				auto iterFind = std::find_if(mPtrsToResolve.begin(), mPtrsToResolve.end(), [objectId](PtrToResolve x) { return x.id == objectId; });

				if(iterFind != mPtrsToResolve.end())
					object = iterFind->field->newObject();
			}
		}

		for(auto iter = mPtrsToResolve.begin(); iter != mPtrsToResolve.end(); ++iter)
		{
			IReflectable* resolvedObject = nullptr;

			PtrToResolve curPtr = *iter;
			if(curPtr.id != 0)
			{
				auto iterFind = mDecodedObjects.find(curPtr.id);
				if(iterFind != mDecodedObjects.end())
					resolvedObject = iterFind->second;
			}

			if(curPtr.field->mIsVectorType)
				curPtr.field->setArrayValue(curPtr.object, curPtr.arrIdx, resolvedObject);
			else
				curPtr.field->setValue(curPtr.object, resolvedObject);
		}
	}

	UINT8* BinarySerializer::encodeInternal(IReflectable* object, UINT32 objectId, UINT8* buffer, UINT32& bufferLength, int* bytesWritten, boost::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
	{
		static const UINT32 META_SIZE = 4; // Meta field size
		static const UINT32 NUM_ELEM_FIELD_SIZE = 4; // Size of the field storing number of array elements
		static const UINT32 COMPLEX_TYPE_SIZE = 4; // Size of the field storing the size of a child complex type

		RTTITypeBase* si = object->getRTTI();

		// Encode object ID if it has one
		UINT32 objectMetaData = encodeObjectMetaData(objectId);
		COPY_TO_BUFFER(&objectMetaData, sizeof(UINT32))

		int numFields = si->getNumFields();
		for(int i = 0; i < numFields; i++)
		{
			RTTIField* curGenericField = si->getField(i);

			// Copy field ID & other meta-data like field size and type
			int metaData = encodeFieldMetaData(curGenericField->mUniqueId, curGenericField->getTypeSize(), 
				curGenericField->mIsVectorType, curGenericField->mType, curGenericField->hasDynamicSize());
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
							IReflectable* childObject = curField->getArrayValue(object, arrIdx); 

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

							buffer = complexTypeToBuffer(&childObject, buffer, bufferLength, bytesWritten, flushBufferCallback);
							if(buffer == nullptr)
								return nullptr;
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

							if((*bytesWritten + typeSize) > bufferLength)
							{
								mTotalBytesWritten += *bytesWritten;
								buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
								if(buffer == nullptr || bufferLength < typeSize) return nullptr;
								*bytesWritten = 0;
							}

							curField->arrayElemToBuffer(object, arrIdx, buffer);
							buffer += typeSize;
							*bytesWritten += typeSize;
						}

						break;
					}
				default:
					CM_EXCEPT(InternalErrorException, 
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
						IReflectable* childObject = curField->getValue(object); 

						UINT32 objId = registerObjectPtr(childObject);
						COPY_TO_BUFFER(&objId, sizeof(UINT32))

						break;
					}
				case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);
						IReflectable& childObject = curField->getValue(object);

						buffer = complexTypeToBuffer(&childObject, buffer, bufferLength, bytesWritten, flushBufferCallback);
						if(buffer == nullptr)
							return nullptr;

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

						if((*bytesWritten + typeSize) > bufferLength)
						{
							mTotalBytesWritten += *bytesWritten;
							buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
							if(buffer == nullptr || bufferLength < typeSize) return nullptr;
							*bytesWritten = 0;
						}


						curField->toBuffer(object, buffer);
						buffer += typeSize;
						*bytesWritten += typeSize;

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
						UINT32 remainingSize = dataBlockSize;
						while(remainingSize > 0)
						{
							UINT32 remainingSpaceInBuffer = bufferLength - *bytesWritten;

							if(remainingSize <= remainingSpaceInBuffer)
							{
								COPY_TO_BUFFER(dataToStore, remainingSize);
								remainingSize = 0;
							}
							else
							{
								memcpy(buffer, dataToStore, remainingSpaceInBuffer);
								buffer += remainingSpaceInBuffer;
								*bytesWritten += remainingSpaceInBuffer;
								dataToStore += remainingSpaceInBuffer;
								remainingSize -= remainingSpaceInBuffer;
								
								mTotalBytesWritten += *bytesWritten;
								buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
								if(buffer == nullptr || bufferLength == 0) return nullptr;
								*bytesWritten = 0;
							}
						}

						break;
					}
				default:
					CM_EXCEPT(InternalErrorException, 
						"Error encoding data. Encountered a type I don't know how to encode. Type: " + toString(UINT32(curGenericField->mType)) + 
						", Is array: " + toString(curGenericField->mIsVectorType));
				}
			}
		}

		return buffer;
	}

	bool BinarySerializer::decodeInternal(IReflectable* object, UINT8* data, UINT32 dataLength, UINT32& bytesRead)
	{
		static const int META_SIZE = 4; // Meta field size
		static const int NUM_ELEM_FIELD_SIZE = 4; // Size of the field storing number of array elements
		static const int COMPLEX_TYPE_FIELD_SIZE = 4; // Size of the field storing the size of a child complex type
		static const int DATA_BLOCK_TYPE_FIELD_SIZE = 4;

		RTTITypeBase* si = nullptr;
		if(object != nullptr)
			si = object->getRTTI();

		if((bytesRead + sizeof(UINT32)) > dataLength)
		{
			CM_EXCEPT(InternalErrorException, 
				"Error decoding data.");
		}

		UINT32 objectMetaData = 0;
		memcpy(&objectMetaData, data, sizeof(UINT32));
		data += sizeof(UINT32);
		bytesRead += sizeof(UINT32);

		UINT32 objectId = 0;
		decodeObjectMetaData(objectMetaData, objectId);

		if(object != nullptr && objectId != 0)
			mDecodedObjects.insert(std::make_pair(objectId, object));
		
		while(bytesRead < dataLength)
		{
			int metaData = -1;

			if((bytesRead + META_SIZE) > dataLength)
			{
				CM_EXCEPT(InternalErrorException, 
					"Error decoding data.");
			}

			memcpy((void*)&metaData, data, META_SIZE);

			if(isObjectMetaData(metaData)) // We've reached a new object
			{
				UINT32 objId = 0;
				decodeObjectMetaData(metaData, objId);

				if(objId != 0) // Objects with 0 ID represent complex types serialized by value, in which case we just decode them on the spot
					return true;
			}

			data += META_SIZE;
			bytesRead += META_SIZE;

			bool isArray;
			SerializableFieldType fieldType;
			UINT16 fieldId;
			UINT8 fieldSize;
			bool hasDynamicSize;
			decodeFieldMetaData(metaData, fieldId, fieldSize, isArray, fieldType, hasDynamicSize);
			
			RTTIField* curGenericField = nullptr;
			
			if(si != nullptr)
				curGenericField = si->findField(fieldId);

			if(curGenericField != nullptr)
			{
				if(curGenericField->getTypeSize() != fieldSize)
				{
					CM_EXCEPT(InternalErrorException, 
						"Data type mismatch. Type size stored in file and actual type size don't match. (" 
						+ toString(curGenericField->getTypeSize()) + " vs. " + toString(fieldSize) + ")");
				}

				if(curGenericField->mIsVectorType != isArray)
				{
					CM_EXCEPT(InternalErrorException, 
						"Data type mismatch. One is array, other is a single type.");
				}

				if(curGenericField->mType != fieldType) 
				{
					CM_EXCEPT(InternalErrorException, 
						"Data type mismatch. Field types don't match. " + toString(UINT32(curGenericField->mType)) + " vs. " + toString(UINT32(fieldType)));
				}
			}

			int arrayNumElems = 1;
			if(isArray)
			{
				if((bytesRead + NUM_ELEM_FIELD_SIZE) > dataLength)
				{
					CM_EXCEPT(InternalErrorException, 
						"Error decoding data.");
				}

				memcpy((void*)&arrayNumElems, data, NUM_ELEM_FIELD_SIZE);
				data += NUM_ELEM_FIELD_SIZE;
				bytesRead += NUM_ELEM_FIELD_SIZE;

				if(curGenericField != nullptr)
					curGenericField->setArraySize(object, arrayNumElems);

				switch(fieldType)
				{
				case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

						for(int i = 0; i < arrayNumElems; i++)
						{
							if((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
							{
								CM_EXCEPT(InternalErrorException, 
									"Error decoding data.");
							}

							int objectId = 0;
							memcpy(&objectId, data, COMPLEX_TYPE_FIELD_SIZE);
							data += COMPLEX_TYPE_FIELD_SIZE;
							bytesRead += COMPLEX_TYPE_FIELD_SIZE;

							if(curField != nullptr)
								mPtrsToResolve.push_back(PtrToResolve(curField, object, objectId, i));
						}

						break;
					}
				case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

						for(int i = 0; i < arrayNumElems; i++)
						{
							if((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
							{
								CM_EXCEPT(InternalErrorException, 
									"Error decoding data.");
							}

							int complexTypeSize = COMPLEX_TYPE_FIELD_SIZE;
							if(curField != nullptr)
							{
								IReflectable* complexType = complexTypeFromBuffer(curField, data, &complexTypeSize);
								curField->setArrayValue(object, i, *complexType);
								delete complexType;
							}

							data += complexTypeSize;
							bytesRead += complexTypeSize;
						}
						break;
					}
				case SerializableFT_Plain:
					{
						RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

						for(int i = 0; i < arrayNumElems; i++)
						{
							UINT32 typeSize = fieldSize;
							if(hasDynamicSize)
								memcpy(&typeSize, data, sizeof(UINT32));

							if(curField != nullptr)
								curField->arrayElemFromBuffer(object, i, data);

							data += typeSize;
							bytesRead += typeSize;
						}
						break;
					}
				default:
					CM_EXCEPT(InternalErrorException, 
						"Error decoding data. Encountered a type I don't know how to decode. Type: " + toString(UINT32(fieldType)) + 
						", Is array: " + toString(isArray));
				}
			}
			else
			{
				switch(fieldType)
				{
				case SerializableFT_ReflectablePtr:
					{
						RTTIReflectablePtrFieldBase* curField = static_cast<RTTIReflectablePtrFieldBase*>(curGenericField);

						if((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
						{
							CM_EXCEPT(InternalErrorException, 
								"Error decoding data.");
						}

						int objectId = 0;
						memcpy(&objectId, data, COMPLEX_TYPE_FIELD_SIZE);
						data += COMPLEX_TYPE_FIELD_SIZE;
						bytesRead += COMPLEX_TYPE_FIELD_SIZE;

						if(curField != nullptr)
							mPtrsToResolve.push_back(PtrToResolve(curField, object, objectId));

						break;
					}
				case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

						if((bytesRead + COMPLEX_TYPE_FIELD_SIZE) > dataLength)
						{
							CM_EXCEPT(InternalErrorException, 
								"Error decoding data.");
						}

						int complexTypeSize = COMPLEX_TYPE_FIELD_SIZE;
						if(curField != nullptr)
						{
							IReflectable* complexType = complexTypeFromBuffer(curField, data, &complexTypeSize);
							curField->setValue(object, *complexType);
							delete complexType;
						}

						data += complexTypeSize;
						bytesRead += complexTypeSize;

						break;
					}
				case SerializableFT_Plain:
					{
						RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);

						UINT32 typeSize = fieldSize;
						if(hasDynamicSize)
							memcpy(&typeSize, data, sizeof(UINT32));

						if(curField != nullptr)
							curField->fromBuffer(object, data);

						data += typeSize;
						bytesRead += typeSize;
						break;
					}
				case SerializableFT_DataBlock:
					{
						RTTIManagedDataBlockFieldBase* curField = static_cast<RTTIManagedDataBlockFieldBase*>(curGenericField);

						if((bytesRead + DATA_BLOCK_TYPE_FIELD_SIZE) > dataLength)
						{
							CM_EXCEPT(InternalErrorException, 
								"Error decoding data.");
						}

						// Data block size
						UINT32 dataBlockSize = 0;
						memcpy(&dataBlockSize, data, DATA_BLOCK_TYPE_FIELD_SIZE);
						data += DATA_BLOCK_TYPE_FIELD_SIZE;
						bytesRead += DATA_BLOCK_TYPE_FIELD_SIZE;

						if((bytesRead + dataBlockSize) > dataLength)
						{
							CM_EXCEPT(InternalErrorException, 
								"Error decoding data.");
						}

						// Data block data
						if(curField != nullptr)
						{
							UINT8* dataCopy = new UINT8[dataBlockSize]; // TODO - Low priority. I need to read files better, so I
							memcpy(dataCopy, data, dataBlockSize);		//    can just pass the buffer pointer directly without copying (possibly large amounts of data)

							ManagedDataBlock value(dataCopy, dataBlockSize, false); // Not managed because I assume the owner class will decide whether to delete the data or keep it
							curField->setValue(object, value);
						}

						data += dataBlockSize;
						bytesRead += dataBlockSize;

						break;
					}
				default:
					CM_EXCEPT(InternalErrorException, 
						"Error decoding data. Encountered a type I don't know how to decode. Type: " + toString(UINT32(fieldType)) + 
						", Is array: " + toString(isArray));
				}
			}
		}

		return false;
	}

	UINT32 BinarySerializer::getObjectSize(IReflectable* object)
	{
		if(object == nullptr)
			return 0;

		UINT32 objectSize = 0;
		RTTITypeBase* si = object->getRTTI();

		// Object ID
		objectSize += sizeof(UINT32);

		int numFields = si->getNumFields();
		for(int i = 0; i < numFields; i++)
		{
			RTTIField* curGenericField = si->getField(i);

			// Field meta data
			objectSize += sizeof(UINT32);

			if(curGenericField->mIsVectorType)
			{
				UINT32 arrayNumElems = curGenericField->getArraySize(object);

				// Num array elems
				objectSize += sizeof(UINT32);

				switch(curGenericField->mType)
				{
					case SerializableFT_ReflectablePtr:
						{
							objectSize += sizeof(UINT32) * arrayNumElems;
							break;
						}
					case SerializableFT_Reflectable:
						{
							RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);

							for(UINT32 arrIdx = 0; arrIdx < arrayNumElems; arrIdx++)
							{
								IReflectable& childObject = curField->getArrayValue(object, arrIdx);
								objectSize += getObjectSize(&childObject);
							}

							break;
						}
					case SerializableFT_Plain:
						{
							RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);
							objectSize += arrayNumElems * curField->getTypeSize();

							break;
						}
					default:
						CM_EXCEPT(InternalErrorException, 
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
						objectSize += sizeof(UINT32);
						break;
					}
				case SerializableFT_Reflectable:
					{
						RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);
						IReflectable& childObject = curField->getValue(object);
						objectSize += getObjectSize(&childObject);

						break;
					}
				case SerializableFT_Plain:
					{
						RTTIPlainFieldBase* curField = static_cast<RTTIPlainFieldBase*>(curGenericField);
						objectSize += curField->getTypeSize();

						break;
					}
				case SerializableFT_DataBlock:
					{
						RTTIManagedDataBlockFieldBase* curField = static_cast<RTTIManagedDataBlockFieldBase*>(curGenericField);
						ManagedDataBlock value = curField->getValue(object);

						// Data block size
						UINT32 dataBlockSize = value.getSize();
						objectSize += sizeof(UINT32) + dataBlockSize;

						break;
					}
				default:
					CM_EXCEPT(InternalErrorException, 
						"Error encoding data. Encountered a type I don't know how to encode. Type: " + toString(UINT32(curGenericField->mType)) + 
						", Is array: " + toString(curGenericField->mIsVectorType));
				}
			}
		}

		return objectSize;
	}

	UINT32 BinarySerializer::encodeFieldMetaData(UINT16 id, UINT8 size, bool array, SerializableFieldType type, bool hasDynamicSize)
	{
		// If O == 0 - Meta contains field information (Encoded using this method)
		//// Encoding: IIII IIII IIII IIII SSSS SSSS xxYP DCAO
		//// I - Id
		//// S - Size
		//// C - Complex
		//// A - Array
		//// D - Data block
		//// P - Complex ptr
		//// O - Object descriptor
		//// Y - Simple field has dynamic size

		return (id << 16 | size << 8 | 
			(array ? 0x02 : 0) | 
			((type == SerializableFT_DataBlock) ? 0x04 : 0) | 
			((type == SerializableFT_Reflectable) ? 0x08 : 0) | 
			((type == SerializableFT_ReflectablePtr) ? 0x10 : 0) | 
			(hasDynamicSize ? 0x20 : 0)); // TODO - Low priority. Technically I could encode this much more tightly, and use var-ints for ID
	}

	void BinarySerializer::decodeFieldMetaData(UINT32 encodedData, UINT16& id, UINT8& size, bool& array, SerializableFieldType& type, bool& hasDynamicSize)
	{
		if(isObjectMetaData(encodedData))
		{
			CM_EXCEPT(InternalErrorException, 
				"Meta data represents an object description but is trying to be decoded as a field descriptor.");
		}

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

	UINT32 BinarySerializer::encodeObjectMetaData(UINT32 objId)
	{
		// If O == 1 - Meta contains object instance information (Encoded using encodeObjectMetaData)
		//// Encoding: SSSS SSSS SSSS SSSS xxxx xxxx xxxx xxxO
		//// S - Size of the object identifier
		//// O - Object descriptor

		return (objId << 1) | 0x01;
	}

	void BinarySerializer::decodeObjectMetaData(UINT32 encodedData, UINT32& objId)
	{
		if(!isObjectMetaData(encodedData))
		{
			CM_EXCEPT(InternalErrorException, 
				"Meta data represents a field description but is trying to be decoded as an object descriptor.");
		}

		objId = (encodedData >> 1) & 0x7FFFFFFF;
	}

	bool BinarySerializer::isObjectMetaData(UINT32 encodedData)
	{
		return ((encodedData & 0x01) != 0);
	}

	UINT8* BinarySerializer::complexTypeToBuffer(IReflectable* object, UINT8* buffer, UINT32& bufferLength, int* bytesWritten, boost::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
	{
		static const UINT32 COMPLEX_TYPE_FIELD_SIZE = 4; // Size of the field storing the size of a child complex type

		int complexTypeSize = 0;
		if(object != nullptr)
			complexTypeSize = getObjectSize(object);

		COPY_TO_BUFFER(&complexTypeSize, COMPLEX_TYPE_FIELD_SIZE)

		if(object != nullptr)
			return encodeInternal(object, 0, buffer, bufferLength, bytesWritten, flushBufferCallback);

		return buffer;
	}

	IReflectable* BinarySerializer::complexTypeFromBuffer(RTTIReflectableFieldBase* field, UINT8* data, int* complexTypeSize)
	{
		static const int COMPLEX_TYPE_FIELD_SIZE = 4; // Size of the field storing the size of a child complex type

		memcpy(complexTypeSize, data, COMPLEX_TYPE_FIELD_SIZE);
		data += COMPLEX_TYPE_FIELD_SIZE;

		IReflectable* emptyObject = nullptr;
		if(*complexTypeSize > 0)
		{
			emptyObject = field->newObject();
			UINT32 dummy = 0;
			decodeInternal(emptyObject, data, *complexTypeSize, dummy);
		}

		*complexTypeSize += COMPLEX_TYPE_FIELD_SIZE;

		return emptyObject;
	}

	UINT32 BinarySerializer::findOrCreatePersistentId(IReflectable* object)
	{
		UINT32 ptrAddress = (UINT32)object;

		auto findIter = mObjectAddrToId.find(ptrAddress);
		if(findIter != mObjectAddrToId.end())
			return findIter->second;

		UINT32 objId = mLastUsedObjectId++;
		mObjectAddrToId.insert(std::make_pair(ptrAddress, objId));

		return objId;
	}

	UINT32 BinarySerializer::registerObjectPtr(IReflectable* object)
	{
		if(object == nullptr)
			return 0;

		UINT32 ptrAddress = (UINT32)object;

		auto iterFind = mObjectAddrToId.find(ptrAddress);
		if(iterFind == mObjectAddrToId.end())
		{
			UINT32 objId = findOrCreatePersistentId(object);

			mObjectsToEncode.push_back(ObjectToEncode(objId, object));
			mObjectAddrToId.insert(std::make_pair(ptrAddress, objId));

			return objId;
		}

		return iterFind->second;
	}
}

#undef COPY_TO_BUFFER