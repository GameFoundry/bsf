#pragma once

#include <string>

#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/static_assert.hpp>
#include <type_traits>

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmManagedDataBlock.h"
#include "CmException.h"

namespace CamelotFramework
{
	class RTTITypeBase;
	struct RTTIField;

	/**
	 * @brief	Types of fields we can serialize:
	 * 			
	 * - Plain - Native data types, POD (Plain old data) structures, or in general types we don't want to (or can't) inherit from IReflectable. 
	 *			  
	 * - DataBlock - Array of bytes of a certain size. When returning a data block you may specify if its managed or unmanaged.  
	 *				 Managed data blocks have their buffers deleted after they go out of scope. This is useful if you need to return some
	 *				 temporary data. On the other hand if the data in the block belongs to your class, and isn't temporary, keep the data unmanaged.
	 *				 
	 * - Reflectable - Field that is of IReflectable type. Cannot be a pointer to IReflectable and must be actual value type. 
	 * 
	 * - ReflectablePtr - A pointer to IReflectable.
	 */
	enum SerializableFieldType
	{
		SerializableFT_Plain,
		SerializableFT_DataBlock,
		SerializableFT_Reflectable,
		SerializableFT_ReflectablePtr
	};

	/**
	 * @brief	Various flags you can assign to RTTI fields.
	 */
	enum RTTIFieldFlag
	{
		// This flag is only used on field types of ReflectablePtr type, and it is used
		// to solve circular references. Circular references cause an issue when deserializing, 
		// as the algorithm doesn't know which object to deserialize first. By making one of 
		// the references weak, you tell the algorithm that it doesn't have to guarantee 
		// the object will be fully deserialized before being assigned to the field.
		//
		// In short: If you make a reference weak, when "set" method of that field is called,
		// it is not guaranteed the value provided is fully initialized, so you should not access any of its
		// data until deserialization is fully complete. You only need to use this flag if the RTTI system
		// complains that is has found a circular reference.
		RTTI_Flag_WeakRef = 0x01
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
		UINT64 mFlags;

		bool isPlainType() { return mType == SerializableFT_Plain; }
		bool isDataBlockType() { return mType == SerializableFT_DataBlock; }
		bool isReflectableType() { return mType == SerializableFT_Reflectable; }
		bool isReflectablePtrType() { return mType == SerializableFT_ReflectablePtr; }
		
		UINT64 getFlags() const { return mFlags; }

		virtual UINT32 getArraySize(void* object) = 0;
		virtual void setArraySize(void* object, UINT32 size) = 0;

		virtual UINT32 getTypeSize() = 0;
		virtual bool hasDynamicSize() = 0;

		/**
		 * @brief	Throws an exception if this field doesn't contain a plain value.
		 *
		 * @param	array	If true then the field must support plain array types.
		 */
		void checkIsPlain(bool array);

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
			std::string mName, UINT16 mUniqueId, bool mIsVectorType, SerializableFieldType type, UINT64 flags)
		{
			this->valueGetter = valueGetter;
			this->valueSetter = valueSetter;
			this->arraySizeGetter = arraySizeGetter;
			this->arraySizeSetter = arraySizeSetter;
			this->mName = mName;
			this->mUniqueId = mUniqueId;
			this->mIsVectorType = mIsVectorType;
			this->mType = type;
			this->mFlags = flags;
		}
	};
}