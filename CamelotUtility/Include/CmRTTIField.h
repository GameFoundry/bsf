#pragma once

#include <string>

#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/static_assert.hpp>

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class RTTITypeBase;
	struct RTTIField;

	template<class T>
	struct SerializableSimpleType 
	{ 
		//BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, // We need this hacky check, otherwise if we only use "false" compiler will evaluate the template and trigger this message 
		//	"Provided data type isn't marked as serializable. Declare SerializableSimpleType template specialization to define a unique ID and needed methods for the type.");

		enum { id = 0 }; 
		enum { hasDynamicSize = 0 };

		static void toMemory(T& data, char* memory)	
		{ 
			memcpy(memory, &data, sizeof(T)); 
		}

		static void fromMemory(T& data, char* memory)
		{
			memcpy(&data, memory, sizeof(T)); 
		}

		static UINT32 getDynamicSize(T& data)
		{ 
			assert(false); 
			return sizeof(T);
		}
	};

#define CM_SERIALIZABLE_SIMPLE_TYPE(type, type_id)				\
	template<> struct SerializableSimpleType<##type##>			\
	{	enum { id=##type_id }; enum { hasDynamicSize = 0 };		\
	static void toMemory(##type##& data, char* memory)		\
	{ memcpy(memory, &data, sizeof(##type##)); }			\
	static void fromMemory(##type##& data, char* memory)	\
	{ memcpy(&data, memory, sizeof(##type##)); }			\
	static UINT32 getDynamicSize(##type##& data)			\
	{ assert(false); return sizeof(##type##); }				\
	}; 

	CM_SERIALIZABLE_SIMPLE_TYPE(UINT8, 1);
	CM_SERIALIZABLE_SIMPLE_TYPE(UINT16, 2);
	CM_SERIALIZABLE_SIMPLE_TYPE(UINT32, 3);
	CM_SERIALIZABLE_SIMPLE_TYPE(UINT64, 4);
	CM_SERIALIZABLE_SIMPLE_TYPE(INT8, 5);
	CM_SERIALIZABLE_SIMPLE_TYPE(INT16, 6);
	CM_SERIALIZABLE_SIMPLE_TYPE(INT32, 7);
	CM_SERIALIZABLE_SIMPLE_TYPE(INT64, 8);
	CM_SERIALIZABLE_SIMPLE_TYPE(float, 9);
	CM_SERIALIZABLE_SIMPLE_TYPE(double, 10);
	CM_SERIALIZABLE_SIMPLE_TYPE(bool, 11);
		
	/**
	 * @brief	Strings need to copy their data in a slightly more intricate way than just memcpy.
	 */
	template<> struct SerializableSimpleType<String>
	{	
		enum { id = 20 }; enum { hasDynamicSize = 1 };

		static void toMemory(String& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);
			memcpy(memory, data.data(), size); 
		}

		static void fromMemory(String& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			size -= sizeof(UINT32);
			char* buffer = new char[size + 1]; // TODO - Use a better allocator
			memcpy(buffer, memory, size); 
			buffer[size] = '\0';
			data = String(buffer);

			delete[] buffer; 
		}

		static UINT32 getDynamicSize(String& data)	
		{ 
			return data.size() * sizeof(String::value_type) + sizeof(UINT32);
		}	
	}; 

	/**
	 * @brief	Types of fields we can serialize:
	 * 			
	 * - Simple - Native data types, POD (Plain old data) structures, or in general types we don't want to (or can't) inherit from IReflectable. 
	 *			  
	 * - DataBlock - Array of bytes of a certain size. When returning a data block you may specify if its managed or unmanaged.  
	 *				 Managed data blocks have their buffers deleted after they go out of scope. This is useful if you need to return some
	 *				 temporary data. On the other hand if the data in the block belongs to your class, and isn't temporary, keep the data unmanaged.
	 *				 
	 * - Complex - Field that is of IReflectable type. Cannot be a pointer to IReflectable and must be actual value type. 
	 * 
	 * - ComplexPtr - A pointer to IReflectable.
	 */
	enum SerializableFieldType
	{
		SerializableFT_Plain,
		SerializableFT_DataBlock,
		SerializableFT_Reflectable,
		SerializableFT_ReflectablePtr
	};

	/**
	 * @brief	Structure that keeps meta-data concerning a single class field. 
	 */
	struct CM_UTILITY_EXPORT RTTIField
	{
		boost::any valueGetter;
		boost::any valueSetter;

		boost::any arraySizeGetter;
		boost::any arraySizeSetter;

		std::string mName;
		UINT16 mUniqueId;
		bool mIsVectorType;
		SerializableFieldType mType;

		bool isSimpleType() { return mType == SerializableFT_Plain; }
		bool isDataBlockType() { return mType == SerializableFT_DataBlock; }
		bool isComplexType() { return mType == SerializableFT_Reflectable; }
		bool isComplexPtrType() { return mType == SerializableFT_ReflectablePtr; }
		
		virtual UINT32 getArraySize(void* object) = 0;
		virtual void setArraySize(void* object, UINT32 size) = 0;

		virtual UINT32 getTypeSize() = 0;
		virtual bool hasDynamicSize() = 0;

		/**
		 * @brief	Throws an exception if this field doesn't contain a simple value.
		 *
		 * @param	array	If true then the field must support simple array types.
		 */
		void checkIsSimple(bool array);

		/**
		 * @brief	Throws an exception if this field doesn't contain a complex value.
		 *
		 * @param	array	If true then the field must support complex array types.
		 */
		void checkIsComplex(bool array);

		/**
		 * @brief	Throws an exception if this field doesn't contain a complex pointer value.
		 *
		 * @param	array	If true then the field must support complex array types.
		 */
		void checkIsComplexPtr(bool array);

		/**
		 * @brief	Throws an exception depending if the field is or isn't an array.
		 *
		 * @param	array	If true, then exception will be thrown if field is not an array.
		 * 					If false, then it will be thrown if field is an array.
		 */
		void checkIsArray(bool array);

		/**
		 * @brief	Throws an exception if this field doesn't contain a data block value.
		 */
		void checkIsDataBlock();

	protected:
		void initAll(boost::any valueGetter, boost::any valueSetter, boost::any arraySizeGetter, boost::any arraySizeSetter,
			std::string mName, UINT16 mUniqueId, bool mIsVectorType, SerializableFieldType type)
		{
			this->valueGetter = valueGetter;
			this->valueSetter = valueSetter;
			this->arraySizeGetter = arraySizeGetter;
			this->arraySizeSetter = arraySizeSetter;
			this->mName = mName;
			this->mUniqueId = mUniqueId;
			this->mIsVectorType = mIsVectorType;
			this->mType = type;
		}
	};
}