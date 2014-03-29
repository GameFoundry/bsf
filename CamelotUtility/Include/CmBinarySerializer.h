#pragma once

#include <unordered_map>
#include <boost/function.hpp>

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"

namespace CamelotFramework
{
	class IReflectable;
	struct RTTIReflectableFieldBase;
	struct RTTIReflectablePtrFieldBase;

	// TODO - Low priority. I will probably want to extract a generalized Serializer class so we can re-use the code
	// in text or other serializers
	// TODO - Low priority. Encode does a chunk-based encode so that we don't need to know the buffer size in advance,
	// and don't have to use a lot of memory for the buffer. Consider doing something similar for decode.
	// TODO - Low priority. Add a simple encode method that doesn't require a callback, instead it calls the callback internally
	// and creates the buffer internally.
	/**
	 * @brief	Encodes all the fields of the provided object into a binary format. Fields are
	 * 			encoded using their unique IDs. Encoded data will remain compatible for decoding even
	 * 			if you modify the encoded class, as long as you assign new unique field IDs to
	 * 			added/modified fields.
	 * 			
	 *			Like for any serializable class, fields are defined in RTTIType that each
	 *			IReflectable class must be able to return.
	 *
	 * 			Any data the object or its children are pointing to will also be serialized (unless the pointer isn't
	 * 			registered in RTTIType). Upon decoding the pointer addresses will be set
	 * 			to proper values.
	 * 			
	 * @note	When deserializing make sure not to access any child members of a deserialized object in your RTTIType class.
	 * 			Objects might be deserialized in an unknown order, so it is not guaranteed that child elements have been
	 * 			deserialized before their parent. 
	 */
	class CM_UTILITY_EXPORT BinarySerializer
	{
	public:
		BinarySerializer();

		/**
		 * @brief	Encodes all serializable fields provided by "object" into a binary format. Data is written in chunks.
		 * 			Whenever a chunk is filled a callback is triggered that gives the user opportunity to expand or 
		 * 			empty the buffer (for example write the chunk to disk)
		 *
		 * @param [in]	object				Object to encode into binary format.
		 * @param [out]	buffer				Preallocated buffer where the data will be stored.
		 * @param	bufferLength			Length of the buffer, in bytes.
		 * @param [out]	bytesWritten		Length of the data that was actually written to the buffer,
		 * 									in bytes.
		 * @param	flushBufferCallback 	This callback will get called whenever the buffer gets full (Be careful to check the provided
		 * 									"bytesRead" variable, as buffer might not be full completely). User must then
		 * 									either create a new buffer or empty the existing one, and then return it by the callback.
		 * 									If the returned buffer address is NULL, encoding is aborted.
		 */
		void encode(IReflectable* object, UINT8* buffer, UINT32 bufferLength, int* bytesWritten, 
			boost::function<UINT8*(UINT8* buffer, int bytesWritten, UINT32& newBufferSize)> flushBufferCallback);

		/**
		 * @brief	Decodes an object from a binary format.
		 *
		 * @param [in]	data  	Binary data to decode.
		 * @param	dataLength	Length of the data.
		 */
		std::shared_ptr<IReflectable> decode(UINT8* data, UINT32 dataLength);

	private:
		struct ObjectToEncode
		{
			ObjectToEncode(UINT32 _objectId, std::shared_ptr<IReflectable> _object)
				:objectId(_objectId), object(_object)
			{ }

			UINT32 objectId;
			std::shared_ptr<IReflectable> object;
		};

		struct ObjectToDecode
		{
			ObjectToDecode(UINT32 _objectId, std::shared_ptr<IReflectable> _object, UINT8* _locationInBuffer, UINT32 _locationInFile)
				:objectId(_objectId), object(_object), locationInBuffer(_locationInBuffer), locationInFile(_locationInFile), isDecoded(false)
			{ }

			UINT32 objectId;
			std::shared_ptr<IReflectable> object;
			UINT8* locationInBuffer;
			UINT32 locationInFile;
			bool isDecoded;
		};

		struct ObjectMetaData
		{
			UINT32 objectMeta;
			UINT32 typeId;
		};

		UnorderedMap<void*, UINT32>::type mObjectAddrToId;
		UINT32 mLastUsedObjectId;
		Vector<ObjectToEncode>::type mObjectsToEncode;
		int mTotalBytesWritten;

		Map<UINT32, ObjectToDecode>::type mObjectMap;

		UINT32 getObjectSize(IReflectable* object);

		/**
		 * @brief	Encodes a single IReflectable object. 
		 */
		UINT8* encodeInternal(IReflectable* object, UINT32 objectId, UINT8* buffer, UINT32& bufferLength, int* bytesWritten, 
			boost::function<UINT8*(UINT8* buffer, int bytesWritten, UINT32& newBufferSize)> flushBufferCallback);

		/**
		 * @brief	Decodes a single IReflectable object.
		 */
		bool decodeInternal(std::shared_ptr<IReflectable> object, UINT8* data, UINT32 dataLength, UINT32& bytesRead);

		/**
		* @brief	Encodes data required for representing a serialized field, into 4 bytes.
		*/
		UINT32 encodeFieldMetaData(UINT16 id, UINT8 size, bool array, SerializableFieldType type, bool hasDynamicSize);

		/**
		* @brief	Decode meta field that was encoded using encodeFieldMetaData.
		*/
		void decodeFieldMetaData(UINT32 encodedData, UINT16& id, UINT8& size, bool& array, SerializableFieldType& type, bool& hasDynamicSize);

		/**
		 * @brief	Encodes data required for representing an object identifier, into 8 bytes.
		 * 			
		 * 			@note		Id can be a maximum of 30 bits, as two bits are reserved.
		 *
		 * @param	objId	   	Unique ID of the object instance.
		 * @param	objTypeId  	Unique ID of the object type.
		 * @param	isBaseClass	true if this object is base class (i.e. just a part of a larger object).
		 */
		ObjectMetaData encodeObjectMetaData(UINT32 objId, UINT32 objTypeId, bool isBaseClass);

		/**
		* @brief	Decode meta field that was encoded using encodeObjectMetaData.
		*/
		void decodeObjectMetaData(ObjectMetaData encodedData, UINT32& objId, UINT32& objTypeId, bool& isBaseClass);

		/**
		 * @brief	Returns true if the provided encoded meta data represents object meta data.
		 */
		bool isObjectMetaData(UINT32 encodedData);

		/**
		 * @brief	Helper method for encoding a complex object and copying its data to a buffer.
		 */
		UINT8* complexTypeToBuffer(IReflectable* object, UINT8* buffer, UINT32& bufferLength, int* bytesWritten, 
			boost::function<UINT8*(UINT8* buffer, int bytesWritten, UINT32& newBufferSize)> flushBufferCallback);

		/**
		 * @brief	Helper method for decoding a complex object from the provided data buffer.
		 */
		std::shared_ptr<IReflectable> complexTypeFromBuffer(RTTIReflectableFieldBase* field, UINT8* data, int* complexTypeSize);

		/**
		 * @brief	Finds an existing, or creates a unique unique identifier for the specified object. 
		 */
		UINT32 findOrCreatePersistentId(IReflectable* object);

		/**
		 * @brief	Finds or creates an id for the provided object and returns it.
		 * 			And it adds the object to a list of objects that need to be encoded,
		 * 			if it's not already there.
		 */
		UINT32 registerObjectPtr(std::shared_ptr<IReflectable> object);
	};
}