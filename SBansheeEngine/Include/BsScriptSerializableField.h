#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableFieldKey : public CM::IReflectable
	{
	public:
		static ScriptSerializableFieldKeyPtr create(CM::UINT16 typeId, CM::UINT16 fieldId);

		CM::UINT16 mTypeId;
		CM::UINT16 mFieldId;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataKeyRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldData : public CM::IReflectable
	{
	public:
		static ScriptSerializableFieldDataPtr create(const ScriptSerializableTypeInfoPtr& typeInfo, void* value);
		virtual void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo) = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataEntry : public CM::IReflectable
	{
	public:
		static ScriptSerializableFieldDataEntryPtr create(const ScriptSerializableFieldKeyPtr& key, const ScriptSerializableFieldDataPtr& value);

		ScriptSerializableFieldKeyPtr mKey;
		ScriptSerializableFieldDataPtr mValue;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataEntryRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataBool : public ScriptSerializableFieldData
	{
	public:
		bool value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataBoolRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataChar : public ScriptSerializableFieldData
	{
	public:
		wchar_t value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataCharRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI8 : public ScriptSerializableFieldData
	{
	public:
		CM::INT8 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI8RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU8 : public ScriptSerializableFieldData
	{
	public:
		CM::UINT8 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU8RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI16 : public ScriptSerializableFieldData
	{
	public:
		CM::INT16 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI16RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU16 : public ScriptSerializableFieldData
	{
	public:
		CM::UINT16 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU16RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI32 : public ScriptSerializableFieldData
	{
	public:
		CM::INT32 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI32RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU32 : public ScriptSerializableFieldData
	{
	public:
		CM::UINT32 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU32RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI64 : public ScriptSerializableFieldData
	{
	public:
		CM::INT64 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI64RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU64 : public ScriptSerializableFieldData
	{
	public:
		CM::UINT64 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU64RTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataFloat : public ScriptSerializableFieldData
	{
	public:
		float value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataFloatRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};


	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataDouble : public ScriptSerializableFieldData
	{
	public:
		double value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataDoubleRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataString : public ScriptSerializableFieldData
	{
	public:
		CM::WString value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataStringRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataResourceRef : public ScriptSerializableFieldData
	{
	public:
		CM::HResource value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataResourceRefRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataGameObjectRef : public ScriptSerializableFieldData
	{
	public:
		CM::HGameObject value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataGameObjectRefRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataObject : public ScriptSerializableFieldData
	{
	public:
		ScriptSerializableObjectPtr value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataObjectRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataArray : public ScriptSerializableFieldData
	{
	public:
		ScriptSerializableArrayPtr value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataArrayRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataList : public ScriptSerializableFieldData
	{
	public:
		ScriptSerializableListPtr value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataListRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataDictionary : public ScriptSerializableFieldData
	{
	public:
		ScriptSerializableDictionaryPtr value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataDictionaryRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}