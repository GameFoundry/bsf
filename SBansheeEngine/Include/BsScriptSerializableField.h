#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableFieldKey : public IReflectable
	{
	public:
		static ScriptSerializableFieldKeyPtr create(UINT16 typeId, UINT16 fieldId);

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

	class BS_SCR_BE_EXPORT ScriptSerializableFieldData : public IReflectable
	{
	public:
		static ScriptSerializableFieldDataPtr create(const ScriptSerializableTypeInfoPtr& typeInfo, MonoObject* value);
		virtual void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo) = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataEntry : public IReflectable
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI8 : public ScriptSerializableFieldData
	{
	public:
		INT8 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI8RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU8 : public ScriptSerializableFieldData
	{
	public:
		UINT8 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU8RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI16 : public ScriptSerializableFieldData
	{
	public:
		INT16 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI16RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU16 : public ScriptSerializableFieldData
	{
	public:
		UINT16 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU16RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI32 : public ScriptSerializableFieldData
	{
	public:
		INT32 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI32RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU32 : public ScriptSerializableFieldData
	{
	public:
		UINT32 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU32RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI64 : public ScriptSerializableFieldData
	{
	public:
		INT64 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataI64RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU64 : public ScriptSerializableFieldData
	{
	public:
		UINT64 value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataU64RTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataString : public ScriptSerializableFieldData
	{
	public:
		WString value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataStringRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataResourceRef : public ScriptSerializableFieldData
	{
	public:
		HResource value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataResourceRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataGameObjectRef : public ScriptSerializableFieldData
	{
	public:
		HGameObject value;

		void* getValue(const ScriptSerializableTypeInfoPtr& typeInfo);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class ScriptSerializableFieldDataGameObjectRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}