#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableFieldKey : public IReflectable
	{
	public:
		static ManagedSerializableFieldKeyPtr create(UINT16 typeId, UINT16 fieldId);

		UINT16 mTypeId;
		UINT16 mFieldId;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataKeyRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldData : public IReflectable
	{
	public:
		static ManagedSerializableFieldDataPtr create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* value);
		virtual void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo) = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataEntry : public IReflectable
	{
	public:
		static ManagedSerializableFieldDataEntryPtr create(const ManagedSerializableFieldKeyPtr& key, const ManagedSerializableFieldDataPtr& value);

		ManagedSerializableFieldKeyPtr mKey;
		ManagedSerializableFieldDataPtr mValue;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataEntryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataBool : public ManagedSerializableFieldData
	{
	public:
		bool value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataBoolRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataChar : public ManagedSerializableFieldData
	{
	public:
		wchar_t value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataCharRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI8 : public ManagedSerializableFieldData
	{
	public:
		INT8 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataI8RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU8 : public ManagedSerializableFieldData
	{
	public:
		UINT8 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataU8RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI16 : public ManagedSerializableFieldData
	{
	public:
		INT16 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataI16RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU16 : public ManagedSerializableFieldData
	{
	public:
		UINT16 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataU16RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI32 : public ManagedSerializableFieldData
	{
	public:
		INT32 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataI32RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU32 : public ManagedSerializableFieldData
	{
	public:
		UINT32 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataU32RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI64 : public ManagedSerializableFieldData
	{
	public:
		INT64 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataI64RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU64 : public ManagedSerializableFieldData
	{
	public:
		UINT64 value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataU64RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataFloat : public ManagedSerializableFieldData
	{
	public:
		float value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataFloatRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};


	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataDouble : public ManagedSerializableFieldData
	{
	public:
		double value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataDoubleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataString : public ManagedSerializableFieldData
	{
	public:
		WString value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataStringRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataResourceRef : public ManagedSerializableFieldData
	{
	public:
		HResource value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataResourceRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataGameObjectRef : public ManagedSerializableFieldData
	{
	public:
		HGameObject value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataGameObjectRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataObject : public ManagedSerializableFieldData
	{
	public:
		ManagedSerializableObjectPtr value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataArray : public ManagedSerializableFieldData
	{
	public:
		ManagedSerializableArrayPtr value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataList : public ManagedSerializableFieldData
	{
	public:
		ManagedSerializableListPtr value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataDictionary : public ManagedSerializableFieldData
	{
	public:
		ManagedSerializableDictionaryPtr value;

		void* getValue(const ManagedSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ManagedSerializableFieldDataDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}