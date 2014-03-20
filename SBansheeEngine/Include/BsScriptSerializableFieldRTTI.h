#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsScriptSerializableField.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableFieldKeyRTTI : public CM::RTTIType<ScriptSerializableFieldKey, CM::IReflectable, ScriptSerializableFieldKeyRTTI>
	{
	private:
		CM::UINT16& getTypeId(ScriptSerializableFieldKey* obj) { return obj->mTypeId; }
		void setTypeId(ScriptSerializableFieldKey* obj, CM::UINT16& val) { obj->mTypeId = val; }

		CM::UINT16& getFieldId(ScriptSerializableFieldKey* obj) { return obj->mFieldId; }
		void setFieldId(ScriptSerializableFieldKey* obj, CM::UINT16& val) { obj->mFieldId = val; }

	public:
		ScriptSerializableFieldKeyRTTI()
		{
			addPlainField("mTypeId", 0, &ScriptSerializableFieldKeyRTTI::getTypeId, &ScriptSerializableFieldKeyRTTI::setTypeId);
			addPlainField("mFieldId", 1, &ScriptSerializableFieldKeyRTTI::getFieldId, &ScriptSerializableFieldKeyRTTI::setFieldId);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldKey";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldKey;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldKey>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataRTTI : public CM::RTTIType<ScriptSerializableFieldData, CM::IReflectable, ScriptSerializableFieldDataRTTI>
	{
	private:

	public:
		ScriptSerializableFieldDataRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldData";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldData;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			CM_EXCEPT(CM::InvalidStateException, "Cannot instantiate an abstract class.");
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataEntryRTTI : public CM::RTTIType<ScriptSerializableFieldDataEntry, CM::IReflectable, ScriptSerializableFieldDataEntryRTTI>
	{
	private:
		ScriptSerializableFieldKeyPtr getKey(ScriptSerializableFieldDataEntry* obj)	{ return obj->mKey; }
		void setKey(ScriptSerializableFieldDataEntry* obj, ScriptSerializableFieldKeyPtr val) { obj->mKey = val; }

		ScriptSerializableFieldDataPtr getValue(ScriptSerializableFieldDataEntry* obj) { return obj->mValue; }
		void setValue(ScriptSerializableFieldDataEntry* obj, ScriptSerializableFieldDataPtr val) { obj->mValue = val; }

	public:
		ScriptSerializableFieldDataEntryRTTI()
		{
			addReflectablePtrField("mKey", 0, &ScriptSerializableFieldDataEntryRTTI::getKey, &ScriptSerializableFieldDataEntryRTTI::setKey);
			addReflectablePtrField("mValue", 1, &ScriptSerializableFieldDataEntryRTTI::getValue, &ScriptSerializableFieldDataEntryRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataEntry";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataEntry;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataEntry>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataBoolRTTI : public CM::RTTIType<ScriptSerializableFieldDataBool, ScriptSerializableFieldData, ScriptSerializableFieldDataBoolRTTI>
	{
	private:
		bool& getValue(ScriptSerializableFieldDataBool* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataBool* obj, bool& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataBoolRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataBoolRTTI::getValue, &ScriptSerializableFieldDataBoolRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataBool";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataBool;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataCharRTTI : public CM::RTTIType<ScriptSerializableFieldDataChar, ScriptSerializableFieldData, ScriptSerializableFieldDataCharRTTI>
	{
	private:
		wchar_t& getValue(ScriptSerializableFieldDataChar* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataChar* obj, wchar_t& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataCharRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataCharRTTI::getValue, &ScriptSerializableFieldDataCharRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataChar";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataChar;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI8RTTI : public CM::RTTIType<ScriptSerializableFieldDataI8, ScriptSerializableFieldData, ScriptSerializableFieldDataI8RTTI>
	{
	private:
		CM::INT8& getValue(ScriptSerializableFieldDataI8* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI8* obj, CM::INT8& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI8RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI8RTTI::getValue, &ScriptSerializableFieldDataI8RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataI8";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI8;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataI8>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU8RTTI : public CM::RTTIType<ScriptSerializableFieldDataU8, ScriptSerializableFieldData, ScriptSerializableFieldDataU8RTTI>
	{
	private:
		CM::UINT8& getValue(ScriptSerializableFieldDataU8* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU8* obj, CM::UINT8& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU8RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU8RTTI::getValue, &ScriptSerializableFieldDataU8RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataU8";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU8;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataU8>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI16RTTI : public CM::RTTIType<ScriptSerializableFieldDataI16, ScriptSerializableFieldData, ScriptSerializableFieldDataI16RTTI>
	{
	private:
		CM::INT16& getValue(ScriptSerializableFieldDataI16* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI16* obj, CM::INT16& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI16RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI16RTTI::getValue, &ScriptSerializableFieldDataI16RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataI16";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI16;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataI16>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU16RTTI : public CM::RTTIType<ScriptSerializableFieldDataU16, ScriptSerializableFieldData, ScriptSerializableFieldDataU16RTTI>
	{
	private:
		CM::UINT16& getValue(ScriptSerializableFieldDataU16* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU16* obj, CM::UINT16& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU16RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU16RTTI::getValue, &ScriptSerializableFieldDataU16RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataU16";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU16;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataU16>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI32RTTI : public CM::RTTIType<ScriptSerializableFieldDataI32, ScriptSerializableFieldData, ScriptSerializableFieldDataI32RTTI>
	{
	private:
		CM::INT32& getValue(ScriptSerializableFieldDataI32* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI32* obj, CM::INT32& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI32RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI32RTTI::getValue, &ScriptSerializableFieldDataI32RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataI32";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI32;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataI32>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU32RTTI : public CM::RTTIType<ScriptSerializableFieldDataU32, ScriptSerializableFieldData, ScriptSerializableFieldDataU32RTTI>
	{
	private:
		CM::UINT32& getValue(ScriptSerializableFieldDataU32* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU32* obj, CM::UINT32& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU32RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU32RTTI::getValue, &ScriptSerializableFieldDataU32RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataU32";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU32;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataU32>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI64RTTI : public CM::RTTIType<ScriptSerializableFieldDataI64, ScriptSerializableFieldData, ScriptSerializableFieldDataI64RTTI>
	{
	private:
		CM::INT64& getValue(ScriptSerializableFieldDataI64* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI64* obj, CM::INT64& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI64RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI64RTTI::getValue, &ScriptSerializableFieldDataI64RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataI64";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI64;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataI64>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU64RTTI : public CM::RTTIType<ScriptSerializableFieldDataU64, ScriptSerializableFieldData, ScriptSerializableFieldDataU64RTTI>
	{
	private:
		CM::UINT64& getValue(ScriptSerializableFieldDataU64* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU64* obj, CM::UINT64& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU64RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU64RTTI::getValue, &ScriptSerializableFieldDataU64RTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataU64";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU64;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataU64>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataFloatRTTI : public CM::RTTIType<ScriptSerializableFieldDataFloat, ScriptSerializableFieldData, ScriptSerializableFieldDataFloatRTTI>
	{
	private:
		float& getValue(ScriptSerializableFieldDataFloat* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataFloat* obj, float& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataFloatRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataFloatRTTI::getValue, &ScriptSerializableFieldDataFloatRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataFloat";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataFloat;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataFloat>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataDoubleRTTI : public CM::RTTIType<ScriptSerializableFieldDataDouble, ScriptSerializableFieldData, ScriptSerializableFieldDataDoubleRTTI>
	{
	private:
		double& getValue(ScriptSerializableFieldDataDouble* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataDouble* obj, double& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataDoubleRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataDoubleRTTI::getValue, &ScriptSerializableFieldDataDoubleRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataDouble";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataDouble;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataDouble>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataStringRTTI : public CM::RTTIType<ScriptSerializableFieldDataString, ScriptSerializableFieldData, ScriptSerializableFieldDataStringRTTI>
	{
	private:
		CM::WString& getValue(ScriptSerializableFieldDataString* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataString* obj, CM::WString& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataStringRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataStringRTTI::getValue, &ScriptSerializableFieldDataStringRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataString";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataString;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataString>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataResourceRefRTTI : public CM::RTTIType<ScriptSerializableFieldDataResourceRef, ScriptSerializableFieldData, ScriptSerializableFieldDataResourceRefRTTI>
	{
	private:
		CM::HResource& getValue(ScriptSerializableFieldDataResourceRef* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataResourceRef* obj, CM::HResource& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataResourceRefRTTI()
		{
			addReflectableField("mValue", 0, &ScriptSerializableFieldDataResourceRefRTTI::getValue, &ScriptSerializableFieldDataResourceRefRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataResourceRef";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataResourceRef;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataGameObjectRefRTTI : public CM::RTTIType<ScriptSerializableFieldDataGameObjectRef, ScriptSerializableFieldData, ScriptSerializableFieldDataGameObjectRefRTTI>
	{
	private:
		CM::HGameObject& getValue(ScriptSerializableFieldDataGameObjectRef* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataGameObjectRef* obj, CM::HGameObject& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataGameObjectRefRTTI()
		{
			addReflectableField("mValue", 0, &ScriptSerializableFieldDataGameObjectRefRTTI::getValue, &ScriptSerializableFieldDataGameObjectRefRTTI::setValue);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SerializableFieldDataGameObjectRef";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataGameObjectRef;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();
		}
	};
}