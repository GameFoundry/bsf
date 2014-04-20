#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsScriptSerializableField.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableFieldKeyRTTI : public RTTIType<ScriptSerializableFieldKey, IReflectable, ScriptSerializableFieldKeyRTTI>
	{
	private:
		UINT16& getTypeId(ScriptSerializableFieldKey* obj) { return obj->mTypeId; }
		void setTypeId(ScriptSerializableFieldKey* obj, UINT16& val) { obj->mTypeId = val; }

		UINT16& getFieldId(ScriptSerializableFieldKey* obj) { return obj->mFieldId; }
		void setFieldId(ScriptSerializableFieldKey* obj, UINT16& val) { obj->mFieldId = val; }

	public:
		ScriptSerializableFieldKeyRTTI()
		{
			addPlainField("mTypeId", 0, &ScriptSerializableFieldKeyRTTI::getTypeId, &ScriptSerializableFieldKeyRTTI::setTypeId);
			addPlainField("mFieldId", 1, &ScriptSerializableFieldKeyRTTI::getFieldId, &ScriptSerializableFieldKeyRTTI::setFieldId);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldKey";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldKey;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldKey>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataRTTI : public RTTIType<ScriptSerializableFieldData, IReflectable, ScriptSerializableFieldDataRTTI>
	{
	private:

	public:
		ScriptSerializableFieldDataRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InvalidStateException, "Cannot instantiate an abstract class.");
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataEntryRTTI : public RTTIType<ScriptSerializableFieldDataEntry, IReflectable, ScriptSerializableFieldDataEntryRTTI>
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

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataEntry";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataEntry;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataEntry>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataBoolRTTI : public RTTIType<ScriptSerializableFieldDataBool, ScriptSerializableFieldData, ScriptSerializableFieldDataBoolRTTI>
	{
	private:
		bool& getValue(ScriptSerializableFieldDataBool* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataBool* obj, bool& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataBoolRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataBoolRTTI::getValue, &ScriptSerializableFieldDataBoolRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataBool";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataBool;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataCharRTTI : public RTTIType<ScriptSerializableFieldDataChar, ScriptSerializableFieldData, ScriptSerializableFieldDataCharRTTI>
	{
	private:
		wchar_t& getValue(ScriptSerializableFieldDataChar* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataChar* obj, wchar_t& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataCharRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataCharRTTI::getValue, &ScriptSerializableFieldDataCharRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataChar";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataChar;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI8RTTI : public RTTIType<ScriptSerializableFieldDataI8, ScriptSerializableFieldData, ScriptSerializableFieldDataI8RTTI>
	{
	private:
		INT8& getValue(ScriptSerializableFieldDataI8* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI8* obj, INT8& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI8RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI8RTTI::getValue, &ScriptSerializableFieldDataI8RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataI8";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI8;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataI8>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU8RTTI : public RTTIType<ScriptSerializableFieldDataU8, ScriptSerializableFieldData, ScriptSerializableFieldDataU8RTTI>
	{
	private:
		UINT8& getValue(ScriptSerializableFieldDataU8* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU8* obj, UINT8& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU8RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU8RTTI::getValue, &ScriptSerializableFieldDataU8RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataU8";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU8;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataU8>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI16RTTI : public RTTIType<ScriptSerializableFieldDataI16, ScriptSerializableFieldData, ScriptSerializableFieldDataI16RTTI>
	{
	private:
		INT16& getValue(ScriptSerializableFieldDataI16* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI16* obj, INT16& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI16RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI16RTTI::getValue, &ScriptSerializableFieldDataI16RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataI16";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI16;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataI16>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU16RTTI : public RTTIType<ScriptSerializableFieldDataU16, ScriptSerializableFieldData, ScriptSerializableFieldDataU16RTTI>
	{
	private:
		UINT16& getValue(ScriptSerializableFieldDataU16* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU16* obj, UINT16& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU16RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU16RTTI::getValue, &ScriptSerializableFieldDataU16RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataU16";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU16;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataU16>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI32RTTI : public RTTIType<ScriptSerializableFieldDataI32, ScriptSerializableFieldData, ScriptSerializableFieldDataI32RTTI>
	{
	private:
		INT32& getValue(ScriptSerializableFieldDataI32* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI32* obj, INT32& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI32RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI32RTTI::getValue, &ScriptSerializableFieldDataI32RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataI32";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI32;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataI32>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU32RTTI : public RTTIType<ScriptSerializableFieldDataU32, ScriptSerializableFieldData, ScriptSerializableFieldDataU32RTTI>
	{
	private:
		UINT32& getValue(ScriptSerializableFieldDataU32* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU32* obj, UINT32& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU32RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU32RTTI::getValue, &ScriptSerializableFieldDataU32RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataU32";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU32;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataU32>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataI64RTTI : public RTTIType<ScriptSerializableFieldDataI64, ScriptSerializableFieldData, ScriptSerializableFieldDataI64RTTI>
	{
	private:
		INT64& getValue(ScriptSerializableFieldDataI64* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataI64* obj, INT64& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataI64RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataI64RTTI::getValue, &ScriptSerializableFieldDataI64RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataI64";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataI64;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataI64>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataU64RTTI : public RTTIType<ScriptSerializableFieldDataU64, ScriptSerializableFieldData, ScriptSerializableFieldDataU64RTTI>
	{
	private:
		UINT64& getValue(ScriptSerializableFieldDataU64* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataU64* obj, UINT64& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataU64RTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataU64RTTI::getValue, &ScriptSerializableFieldDataU64RTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataU64";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataU64;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataU64>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataFloatRTTI : public RTTIType<ScriptSerializableFieldDataFloat, ScriptSerializableFieldData, ScriptSerializableFieldDataFloatRTTI>
	{
	private:
		float& getValue(ScriptSerializableFieldDataFloat* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataFloat* obj, float& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataFloatRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataFloatRTTI::getValue, &ScriptSerializableFieldDataFloatRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataFloat";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataFloat;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataFloat>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataDoubleRTTI : public RTTIType<ScriptSerializableFieldDataDouble, ScriptSerializableFieldData, ScriptSerializableFieldDataDoubleRTTI>
	{
	private:
		double& getValue(ScriptSerializableFieldDataDouble* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataDouble* obj, double& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataDoubleRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataDoubleRTTI::getValue, &ScriptSerializableFieldDataDoubleRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataDouble";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataDouble;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataDouble>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataStringRTTI : public RTTIType<ScriptSerializableFieldDataString, ScriptSerializableFieldData, ScriptSerializableFieldDataStringRTTI>
	{
	private:
		WString& getValue(ScriptSerializableFieldDataString* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataString* obj, WString& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataStringRTTI()
		{
			addPlainField("mValue", 0, &ScriptSerializableFieldDataStringRTTI::getValue, &ScriptSerializableFieldDataStringRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataString";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataString;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataString>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataResourceRefRTTI : public RTTIType<ScriptSerializableFieldDataResourceRef, ScriptSerializableFieldData, ScriptSerializableFieldDataResourceRefRTTI>
	{
	private:
		HResource& getValue(ScriptSerializableFieldDataResourceRef* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataResourceRef* obj, HResource& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataResourceRefRTTI()
		{
			addReflectableField("mValue", 0, &ScriptSerializableFieldDataResourceRefRTTI::getValue, &ScriptSerializableFieldDataResourceRefRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataResourceRef";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataResourceRef;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataGameObjectRefRTTI : public RTTIType<ScriptSerializableFieldDataGameObjectRef, ScriptSerializableFieldData, ScriptSerializableFieldDataGameObjectRefRTTI>
	{
	private:
		HGameObject& getValue(ScriptSerializableFieldDataGameObjectRef* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataGameObjectRef* obj, HGameObject& val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataGameObjectRefRTTI()
		{
			addReflectableField("mValue", 0, &ScriptSerializableFieldDataGameObjectRefRTTI::getValue, &ScriptSerializableFieldDataGameObjectRefRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataGameObjectRef";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataGameObjectRef;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataObjectRTTI : public RTTIType<ScriptSerializableFieldDataObject, ScriptSerializableFieldData, ScriptSerializableFieldDataObjectRTTI>
	{
	private:
		ScriptSerializableObjectPtr getValue(ScriptSerializableFieldDataObject* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataObject* obj, ScriptSerializableObjectPtr val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataObjectRTTI()
		{
			addReflectablePtrField("mValue", 0, &ScriptSerializableFieldDataObjectRTTI::getValue, &ScriptSerializableFieldDataObjectRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataObject>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataArrayRTTI : public RTTIType<ScriptSerializableFieldDataArray, ScriptSerializableFieldData, ScriptSerializableFieldDataArrayRTTI>
	{
	private:
		ScriptSerializableArrayPtr getValue(ScriptSerializableFieldDataArray* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataArray* obj, ScriptSerializableArrayPtr val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataArrayRTTI()
		{
			addReflectablePtrField("mValue", 0, &ScriptSerializableFieldDataArrayRTTI::getValue, &ScriptSerializableFieldDataArrayRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataArray";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataArray;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataArray>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataListRTTI : public RTTIType<ScriptSerializableFieldDataList, ScriptSerializableFieldData, ScriptSerializableFieldDataListRTTI>
	{
	private:
		ScriptSerializableListPtr getValue(ScriptSerializableFieldDataList* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataList* obj, ScriptSerializableListPtr val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataListRTTI()
		{
			addReflectablePtrField("mValue", 0, &ScriptSerializableFieldDataListRTTI::getValue, &ScriptSerializableFieldDataListRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataList";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataList;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataList>();
		}
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldDataDictionaryRTTI : public RTTIType<ScriptSerializableFieldDataDictionary, ScriptSerializableFieldData, ScriptSerializableFieldDataDictionaryRTTI>
	{
	private:
		ScriptSerializableDictionaryPtr getValue(ScriptSerializableFieldDataDictionary* obj) { return obj->value; }
		void setValue(ScriptSerializableFieldDataDictionary* obj, ScriptSerializableDictionaryPtr val) { obj->value = val; }

	public:
		ScriptSerializableFieldDataDictionaryRTTI()
		{
			addReflectablePtrField("mValue", 0, &ScriptSerializableFieldDataDictionaryRTTI::getValue, &ScriptSerializableFieldDataDictionaryRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SerializableFieldDataDictionary";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SerializableFieldDataDictionary;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return cm_shared_ptr<ScriptSerializableFieldDataDictionary>();
		}
	};
}