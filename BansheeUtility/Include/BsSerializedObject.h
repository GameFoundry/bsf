#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for intermediate representations of
	 *			objects that are being decoded with BinarySerializer.
	 */
	struct BS_UTILITY_EXPORT SerializedInstance : IReflectable
	{
		virtual ~SerializedInstance() { }
		virtual SPtr<SerializedInstance> clone(bool cloneData = true) = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedInstanceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	A single serialized entry representing a
	 *			and its value in an object.
	 */
	struct BS_UTILITY_EXPORT SerializedEntry : IReflectable
	{
		SerializedEntry()
			:fieldId(0), serialized(nullptr)
		{ }

		UINT32 fieldId;
		SPtr<SerializedInstance> serialized;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedEntryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	A serialized value representing a single entry in an array.
	 */
	struct BS_UTILITY_EXPORT SerializedArrayEntry : IReflectable
	{
		SerializedArrayEntry()
			:index(0), serialized(nullptr)
		{ }

		UINT32 index;
		SPtr<SerializedInstance> serialized;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedArrayEntryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	A serialized portion of an object consisting of multiple entries,
	 *			one for each field.
	 */
	struct BS_UTILITY_EXPORT SerializedSubObject : IReflectable
	{
		SerializedSubObject()
			:typeId(0)
		{ }

		UINT32 typeId;
		UnorderedMap<UINT32, SerializedEntry> entries;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedSubObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};


	/**
	 * @brief	A serialized object consisting of multiple sub-objects,
	 *			one for each inherited class.
	 */
	struct BS_UTILITY_EXPORT SerializedObject : SerializedInstance
	{
		UINT32 getRootTypeId() const { return subObjects[0].typeId; }
		SPtr<SerializedInstance> clone(bool cloneData = true) override;

		Vector<SerializedSubObject> subObjects;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains a pointer to the serialized value of a specific
	 *			field or array entry.
	 */
	struct BS_UTILITY_EXPORT SerializedField : SerializedInstance
	{
		SerializedField()
			:value(nullptr), size(0), ownsMemory(false)
		{

		}

		~SerializedField()
		{
			if (ownsMemory && value != nullptr)
				bs_free(value);
		}

		SPtr<SerializedInstance> clone(bool cloneData = true) override;

		UINT8* value;
		UINT32 size;
		bool ownsMemory;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedFieldRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	A serialized array containing a list of all its entries.
	 */
	struct BS_UTILITY_EXPORT SerializedArray : SerializedInstance
	{
		SerializedArray()
			:numElements(0)
		{ }

		SPtr<SerializedInstance> clone(bool cloneData = true) override;

		UnorderedMap<UINT32, SerializedArrayEntry> entries;
		UINT32 numElements;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SerializedArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}