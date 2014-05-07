#include "CmBinarySerializer.h"

#include "CmException.h"
#include "CmDebug.h"
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

	void BinarySerializer::encode(IReflectable* object, UINT8* buffer, UINT32 bufferLength, int* bytesWritten, std::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
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

				buffer = encodeInternal(curObject.get(), curObjectid, buffer, bufferLength, bytesWritten, flushBufferCallback);
				if(buffer == nullptr)
				{
					CM_EXCEPT(InternalErrorException, 
						"Destination buffer is null or not large enough.");
				}

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

		mObjectsToEncode.clear();
		mObjectAddrToId.clear();
	}

	std::shared_ptr<IReflectable> BinarySerializer::decode(UINT8* data, UINT32 dataLength)
	{
		mObjectMap.clear();

		// Create empty instances of all ptr objects
		UINT32 bytesRead = 0;
		UINT8* dataIter = nullptr;
		std::shared_ptr<IReflectable> rootObject = nullptr;
		do 
		{
			dataIter = data + bytesRead;

			if(sizeof(UINT32) > dataLength)
			{
				CM_EXCEPT(InternalErrorException, 
					"Error decoding data.");
			}

			ObjectMetaData objectMetaData;
			objectMetaData.objectMeta = 0;
			objectMetaData.typeId = 0;
			memcpy(&objectMetaData, dataIter, sizeof(ObjectMetaData));

			UINT32 objectId = 0;
			UINT32 objectTypeId = 0;
			bool isBaseClass = false;
			decodeObjectMetaData(objectMetaData, objectId, objectTypeId, isBaseClass);

			if(isBaseClass)
			{
				CM_EXCEPT(InternalErrorException, "Encountered a base-class object while looking for a new object. " \
					"Base class objects are only supposed to be parts of a larger object.");
			}

			std::shared_ptr<IReflectable> object = IReflectable::createInstanceFromTypeId(objectTypeId);
			mObjectMap.insert(std::make_pair(objectId, ObjectToDecode(objectId, object, dataIter, bytesRead)));

			if(rootObject == nullptr)
				rootObject = object;

		} while (decodeInternal(nullptr, dataIter, dataLength, bytesRead));

		// Now go through all of the objects and actually decode them
		for(auto iter = mObjectMap.begin(); iter != mObjectMap.end(); ++iter)
		{
			ObjectToDecode& objToDecode = iter->second;

			if(objToDecode.isDecoded)
				continue;

			UINT32 objectBytesRead = objToDecode.locationInFile;
			decodeInternal(objToDecode.object, objToDecode.locationInBuffer, dataLength, objectBytesRead);
		}

		mObjectMap.clear();

		return rootObject;
	}

	UINT8* BinarySerializer::encodeInternal(IReflectable* object, UINT32 objectId, UINT8* buffer, UINT32& bufferLength, 
		int* bytesWritten, std::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
	{
		static const UINT32 META_SIZE = 4; // Meta field size
		static const UINT32 NUM_ELEM_FIELD_SIZE = 4; // Size of the field storing number of array elements
		static const UINT32 COMPLEX_TYPE_SIZE = 4; // Size of the field storing the size of a child complex type

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
								std::shared_ptr<IReflectable> childObject = curField->getArrayValue(object, arrIdx); 

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

								if((*bytesWritten + typeSize) > bufferLength)
								{
									mTotalBytesWritten += *bytesWritten;
									buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
									if(buffer == nullptr || bufferLength < typeSize)
									{
										return nullptr;
										si->onSerializationEnded(object);
									}

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
							std::shared_ptr<IReflectable> childObject = curField->getValue(object); 

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

							if((*bytesWritten + typeSize) > bufferLength)
							{
								mTotalBytesWritten += *bytesWritten;
								buffer = flushBufferCallback(buffer - *bytesWritten, *bytesWritten, bufferLength);
								if(buffer == nullptr || bufferLength < typeSize)
								{
									si->onSerializationEnded(object);
									return nullptr;
								}
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
									if(buffer == nullptr || bufferLength == 0) 
									{
										si->onSerializationEnded(object);
										return nullptr;
									}
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

			si->onSerializationEnded(object);

			si = si->getBaseClass();
			isBaseClass = true;

		} while(si != nullptr); // Repeat until we reach the top of the inheritance hierarchy

		return buffer;
	}

	bool BinarySerializer::decodeInternal(std::shared_ptr<IReflectable> object, UINT8* data, UINT32 dataLength, UINT32& bytesRead)
	{
		static const int META_SIZE = 4; // Meta field size
		static const int NUM_ELEM_FIELD_SIZE = 4; // Size of the field storing number of array elements
		static const int COMPLEX_TYPE_FIELD_SIZE = 4; // Size of the field storing the size of a child complex type
		static const int DATA_BLOCK_TYPE_FIELD_SIZE = 4;

		bool moreObjectsToProcess = false;

		RTTITypeBase* si = nullptr;
		if(object != nullptr)
		{
			si = object->getRTTI();

			if(si != nullptr)
				si->onDeserializationStarted(object.get());
		}

		if((bytesRead + sizeof(ObjectMetaData)) > dataLength)
		{
			CM_EXCEPT(InternalErrorException, 
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
				if((bytesRead + sizeof(ObjectMetaData)) > dataLength)
				{
					CM_EXCEPT(InternalErrorException, 
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
				if(objIsBaseClass)
				{
					if(si != nullptr)
						si = si->getBaseClass();

					// Saved and current base classes don't match, so just skip over all that data
					if(si == nullptr || si->getRTTIId() != objTypeId)
					{
						si = nullptr;	
					}

					if(si != nullptr)
					{
						si->onDeserializationStarted(object.get());
					}

					data += sizeof(ObjectMetaData);
					bytesRead += sizeof(ObjectMetaData);
					continue;
				}
				else
				{
					if(objId != 0) 
					{
						moreObjectsToProcess = true; // New object, break out of this method and begin processing it from scratch
						goto exit;
					}

					// Objects with ID == 0 represent complex types serialized by value, but they should only get serialized
					// if we encounter a field with one, not by just iterating through the file.
					CM_EXCEPT(InternalErrorException, "Object with ID 0 encountered. Cannot proceed with serialization.");
				}
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
				if(!hasDynamicSize && curGenericField->getTypeSize() != fieldSize)
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
					curGenericField->setArraySize(object.get(), arrayNumElems);

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
							{
								auto findObj = mObjectMap.find(objectId);

								if(findObj == mObjectMap.end())
								{
									if(objectId != 0)
										LOGWRN("When deserializing, object ID: " + toString(objectId) + " was found but no such object was contained in the file.");

									curField->setArrayValue(object.get(), i, nullptr);
								}
								else
								{
									ObjectToDecode& objToDecode = findObj->second;

									bool needsDecoding = (curField->getFlags() & RTTI_Flag_WeakRef) == 0 && !objToDecode.isDecoded;
									if(needsDecoding)
									{
										UINT32 objectBytesRead = objToDecode.locationInFile;
										decodeInternal(objToDecode.object, objToDecode.locationInBuffer, dataLength, objectBytesRead);

										objToDecode.isDecoded = true;
									}

									curField->setArrayValue(object.get(), i, objToDecode.object);
								}
							}
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

							int complexTypeSize = 0;
							if(curField != nullptr)
							{
								std::shared_ptr<IReflectable> complexType = complexTypeFromBuffer(curField, data, &complexTypeSize);
								curField->setArrayValue(object.get(), i, *complexType);
							}
							else
							{
								memcpy(&complexTypeSize, data, COMPLEX_TYPE_FIELD_SIZE);
								complexTypeSize += COMPLEX_TYPE_FIELD_SIZE;
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
								curField->arrayElemFromBuffer(object.get(), i, data);

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
						{
							auto findObj = mObjectMap.find(objectId);

							if(findObj == mObjectMap.end())
							{
								if(objectId != 0)
									LOGWRN("When deserializing, object ID: " + toString(objectId) + " was found but no such object was contained in the file.");

								curField->setValue(object.get(), nullptr);
							}
							else
							{
								ObjectToDecode& objToDecode = findObj->second;

								bool needsDecoding = (curField->getFlags() & RTTI_Flag_WeakRef) == 0 && !objToDecode.isDecoded;
								if(needsDecoding)
								{
									UINT32 objectBytesRead = objToDecode.locationInFile;
									decodeInternal(objToDecode.object, objToDecode.locationInBuffer, dataLength, objectBytesRead);

									objToDecode.isDecoded = true;
								}

								curField->setValue(object.get(), objToDecode.object);
							}
						}

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

						int complexTypeSize = 0;
						if(curField != nullptr)
						{
							std::shared_ptr<IReflectable> complexType = complexTypeFromBuffer(curField, data, &complexTypeSize);
							curField->setValue(object.get(), *complexType);
						}
						else
						{
							memcpy(&complexTypeSize, data, COMPLEX_TYPE_FIELD_SIZE);
							complexTypeSize += COMPLEX_TYPE_FIELD_SIZE;
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
							curField->fromBuffer(object.get(), data);

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
							UINT8* dataCopy = curField->allocate(object.get(), dataBlockSize); // TODO - Low priority. I need to read files better, so I
							memcpy(dataCopy, data, dataBlockSize);		//    can just pass the buffer pointer directly without copying (possibly large amounts of data)

							ManagedDataBlock value(dataCopy, dataBlockSize); // Not managed because I assume the owner class will decide whether to delete the data or keep it
							curField->setValue(object.get(), value);
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

		moreObjectsToProcess = false;

exit:
		// Finish serialization (in reverse order then it was started)
		if(object != nullptr)
		{
			Stack<RTTITypeBase*> typesToProcess;

			RTTITypeBase* currentType = object->getRTTI();
			while(currentType != nullptr)
			{
				typesToProcess.push(currentType);
				currentType = currentType->getBaseClass();
			}

			while(!typesToProcess.empty())
			{
				currentType = typesToProcess.top();
				typesToProcess.pop();

				currentType->onDeserializationEnded(object.get());
			}
		}

		return moreObjectsToProcess;
	}

	// TODO - This needs serious fixing, it doesn't account for all properties
	UINT32 BinarySerializer::getObjectSize(IReflectable* object)
	{
		if(object == nullptr)
			return 0;

		UINT32 objectSize = 0;
		RTTITypeBase* si = object->getRTTI();

		do 
		{
			// Object ID + type data
			objectSize += sizeof(ObjectMetaData);

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
									objectSize += sizeof(UINT32); // Complex type size
									objectSize += getObjectSize(&childObject);
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

									objectSize += typeSize;
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
							objectSize += sizeof(UINT32);
							break;
						}
					case SerializableFT_Reflectable:
						{
							RTTIReflectableFieldBase* curField = static_cast<RTTIReflectableFieldBase*>(curGenericField);
							IReflectable& childObject = curField->getValue(object);

							objectSize += sizeof(UINT32); // Complex type size
							objectSize += getObjectSize(&childObject);

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

							objectSize += typeSize;

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

			si = si->getBaseClass();

		} while (si != nullptr);

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
		//// Y - Plain field has dynamic size

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

	BinarySerializer::ObjectMetaData BinarySerializer::encodeObjectMetaData(UINT32 objId, UINT32 objTypeId, bool isBaseClass)
	{
		// If O == 1 - Meta contains object instance information (Encoded using encodeObjectMetaData)
		//// Encoding: SSSS SSSS SSSS SSSS xxxx xxxx xxxx xxBO
		//// S - Size of the object identifier
		//// O - Object descriptor
		//// B - Base class indicator
		
		if(objId > 1073741823)
		{
			CM_EXCEPT(InvalidParametersException, "Object ID is larger than we can store (max 30 bits): " + toString(objId));
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
			CM_EXCEPT(InternalErrorException, 
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
		int* bytesWritten, std::function<UINT8*(UINT8*, int, UINT32&)> flushBufferCallback)
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

	std::shared_ptr<IReflectable> BinarySerializer::complexTypeFromBuffer(RTTIReflectableFieldBase* field, UINT8* data, int* complexTypeSize)
	{
		static const int COMPLEX_TYPE_FIELD_SIZE = 4; // Size of the field storing the size of a child complex type

		memcpy(complexTypeSize, data, COMPLEX_TYPE_FIELD_SIZE);
		data += COMPLEX_TYPE_FIELD_SIZE;

		std::shared_ptr<IReflectable> emptyObject = nullptr;
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