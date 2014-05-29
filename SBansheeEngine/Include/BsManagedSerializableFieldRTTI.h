#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmRTTIType.h"
#include "BsManagedSerializableField.h"
#include "CmException.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableFieldKeyRTTI : public RTTIType<ManagedSerializableFieldKey, IReflectable, ManagedSerializableFieldKeyRTTI>
	{
	private:
		UINT16& getTypeId(ManagedSerializableFieldKey* obj) { return obj->mTypeId; }
		void setTypeId(ManagedSerializableFieldKey* obj, UINT16& val) { obj->mTypeId = val; }

		UINT16& getFieldId(ManagedSerializableFieldKey* obj) { return obj->mFieldId; }
		void setFieldId(ManagedSerializableFieldKey* obj, UINT16& val) { obj->mFieldId = val; }

	public:
		ManagedSerializableFieldKeyRTTI()
		{
			addPlainField("mTypeId", 0, &ManagedSerializableFieldKeyRTTI::getTypeId, &ManagedSerializableFieldKeyRTTI::setTypeId);
			addPlainField("mFieldId", 1, &ManagedSerializableFieldKeyRTTI::getFieldId, &ManagedSerializableFieldKeyRTTI::setFieldId);
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
			return bs_shared_ptr<ManagedSerializableFieldKey>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataRTTI : public RTTIType<ManagedSerializableFieldData, IReflectable, ManagedSerializableFieldDataRTTI>
	{
	private:

	public:
		ManagedSerializableFieldDataRTTI()
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
			BS_EXCEPT(InvalidStateException, "Cannot instantiate an abstract class.");
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataEntryRTTI : public RTTIType<ManagedSerializableFieldDataEntry, IReflectable, ManagedSerializableFieldDataEntryRTTI>
	{
	private:
		ManagedSerializableFieldKeyPtr getKey(ManagedSerializableFieldDataEntry* obj)	{ return obj->mKey; }
		void setKey(ManagedSerializableFieldDataEntry* obj, ManagedSerializableFieldKeyPtr val) { obj->mKey = val; }

		ManagedSerializableFieldDataPtr getValue(ManagedSerializableFieldDataEntry* obj) { return obj->mValue; }
		void setValue(ManagedSerializableFieldDataEntry* obj, ManagedSerializableFieldDataPtr val) { obj->mValue = val; }

	public:
		ManagedSerializableFieldDataEntryRTTI()
		{
			addReflectablePtrField("mKey", 0, &ManagedSerializableFieldDataEntryRTTI::getKey, &ManagedSerializableFieldDataEntryRTTI::setKey);
			addReflectablePtrField("mValue", 1, &ManagedSerializableFieldDataEntryRTTI::getValue, &ManagedSerializableFieldDataEntryRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataEntry>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataBoolRTTI : public RTTIType<ManagedSerializableFieldDataBool, ManagedSerializableFieldData, ManagedSerializableFieldDataBoolRTTI>
	{
	private:
		bool& getValue(ManagedSerializableFieldDataBool* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataBool* obj, bool& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataBoolRTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataBoolRTTI::getValue, &ManagedSerializableFieldDataBoolRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataCharRTTI : public RTTIType<ManagedSerializableFieldDataChar, ManagedSerializableFieldData, ManagedSerializableFieldDataCharRTTI>
	{
	private:
		wchar_t& getValue(ManagedSerializableFieldDataChar* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataChar* obj, wchar_t& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataCharRTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataCharRTTI::getValue, &ManagedSerializableFieldDataCharRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataBool>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI8RTTI : public RTTIType<ManagedSerializableFieldDataI8, ManagedSerializableFieldData, ManagedSerializableFieldDataI8RTTI>
	{
	private:
		INT8& getValue(ManagedSerializableFieldDataI8* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataI8* obj, INT8& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataI8RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataI8RTTI::getValue, &ManagedSerializableFieldDataI8RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataI8>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU8RTTI : public RTTIType<ManagedSerializableFieldDataU8, ManagedSerializableFieldData, ManagedSerializableFieldDataU8RTTI>
	{
	private:
		UINT8& getValue(ManagedSerializableFieldDataU8* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataU8* obj, UINT8& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataU8RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataU8RTTI::getValue, &ManagedSerializableFieldDataU8RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataU8>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI16RTTI : public RTTIType<ManagedSerializableFieldDataI16, ManagedSerializableFieldData, ManagedSerializableFieldDataI16RTTI>
	{
	private:
		INT16& getValue(ManagedSerializableFieldDataI16* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataI16* obj, INT16& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataI16RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataI16RTTI::getValue, &ManagedSerializableFieldDataI16RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataI16>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU16RTTI : public RTTIType<ManagedSerializableFieldDataU16, ManagedSerializableFieldData, ManagedSerializableFieldDataU16RTTI>
	{
	private:
		UINT16& getValue(ManagedSerializableFieldDataU16* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataU16* obj, UINT16& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataU16RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataU16RTTI::getValue, &ManagedSerializableFieldDataU16RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataU16>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI32RTTI : public RTTIType<ManagedSerializableFieldDataI32, ManagedSerializableFieldData, ManagedSerializableFieldDataI32RTTI>
	{
	private:
		INT32& getValue(ManagedSerializableFieldDataI32* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataI32* obj, INT32& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataI32RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataI32RTTI::getValue, &ManagedSerializableFieldDataI32RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataI32>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU32RTTI : public RTTIType<ManagedSerializableFieldDataU32, ManagedSerializableFieldData, ManagedSerializableFieldDataU32RTTI>
	{
	private:
		UINT32& getValue(ManagedSerializableFieldDataU32* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataU32* obj, UINT32& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataU32RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataU32RTTI::getValue, &ManagedSerializableFieldDataU32RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataU32>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataI64RTTI : public RTTIType<ManagedSerializableFieldDataI64, ManagedSerializableFieldData, ManagedSerializableFieldDataI64RTTI>
	{
	private:
		INT64& getValue(ManagedSerializableFieldDataI64* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataI64* obj, INT64& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataI64RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataI64RTTI::getValue, &ManagedSerializableFieldDataI64RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataI64>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataU64RTTI : public RTTIType<ManagedSerializableFieldDataU64, ManagedSerializableFieldData, ManagedSerializableFieldDataU64RTTI>
	{
	private:
		UINT64& getValue(ManagedSerializableFieldDataU64* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataU64* obj, UINT64& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataU64RTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataU64RTTI::getValue, &ManagedSerializableFieldDataU64RTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataU64>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataFloatRTTI : public RTTIType<ManagedSerializableFieldDataFloat, ManagedSerializableFieldData, ManagedSerializableFieldDataFloatRTTI>
	{
	private:
		float& getValue(ManagedSerializableFieldDataFloat* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataFloat* obj, float& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataFloatRTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataFloatRTTI::getValue, &ManagedSerializableFieldDataFloatRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataFloat>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataDoubleRTTI : public RTTIType<ManagedSerializableFieldDataDouble, ManagedSerializableFieldData, ManagedSerializableFieldDataDoubleRTTI>
	{
	private:
		double& getValue(ManagedSerializableFieldDataDouble* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataDouble* obj, double& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataDoubleRTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataDoubleRTTI::getValue, &ManagedSerializableFieldDataDoubleRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataDouble>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataStringRTTI : public RTTIType<ManagedSerializableFieldDataString, ManagedSerializableFieldData, ManagedSerializableFieldDataStringRTTI>
	{
	private:
		WString& getValue(ManagedSerializableFieldDataString* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataString* obj, WString& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataStringRTTI()
		{
			addPlainField("mValue", 0, &ManagedSerializableFieldDataStringRTTI::getValue, &ManagedSerializableFieldDataStringRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataString>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataResourceRefRTTI : public RTTIType<ManagedSerializableFieldDataResourceRef, ManagedSerializableFieldData, ManagedSerializableFieldDataResourceRefRTTI>
	{
	private:
		HResource& getValue(ManagedSerializableFieldDataResourceRef* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataResourceRef* obj, HResource& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataResourceRefRTTI()
		{
			addReflectableField("mValue", 0, &ManagedSerializableFieldDataResourceRefRTTI::getValue, &ManagedSerializableFieldDataResourceRefRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataResourceRef>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataGameObjectRefRTTI : public RTTIType<ManagedSerializableFieldDataGameObjectRef, ManagedSerializableFieldData, ManagedSerializableFieldDataGameObjectRefRTTI>
	{
	private:
		HGameObject& getValue(ManagedSerializableFieldDataGameObjectRef* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataGameObjectRef* obj, HGameObject& val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataGameObjectRefRTTI()
		{
			addReflectableField("mValue", 0, &ManagedSerializableFieldDataGameObjectRefRTTI::getValue, &ManagedSerializableFieldDataGameObjectRefRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataGameObjectRef>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataObjectRTTI : public RTTIType<ManagedSerializableFieldDataObject, ManagedSerializableFieldData, ManagedSerializableFieldDataObjectRTTI>
	{
	private:
		ManagedSerializableObjectPtr getValue(ManagedSerializableFieldDataObject* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataObject* obj, ManagedSerializableObjectPtr val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataObjectRTTI()
		{
			addReflectablePtrField("mValue", 0, &ManagedSerializableFieldDataObjectRTTI::getValue, &ManagedSerializableFieldDataObjectRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataObject>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataArrayRTTI : public RTTIType<ManagedSerializableFieldDataArray, ManagedSerializableFieldData, ManagedSerializableFieldDataArrayRTTI>
	{
	private:
		ManagedSerializableArrayPtr getValue(ManagedSerializableFieldDataArray* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataArray* obj, ManagedSerializableArrayPtr val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataArrayRTTI()
		{
			addReflectablePtrField("mValue", 0, &ManagedSerializableFieldDataArrayRTTI::getValue, &ManagedSerializableFieldDataArrayRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataArray>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataListRTTI : public RTTIType<ManagedSerializableFieldDataList, ManagedSerializableFieldData, ManagedSerializableFieldDataListRTTI>
	{
	private:
		ManagedSerializableListPtr getValue(ManagedSerializableFieldDataList* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataList* obj, ManagedSerializableListPtr val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataListRTTI()
		{
			addReflectablePtrField("mValue", 0, &ManagedSerializableFieldDataListRTTI::getValue, &ManagedSerializableFieldDataListRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataList>();
		}
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldDataDictionaryRTTI : public RTTIType<ManagedSerializableFieldDataDictionary, ManagedSerializableFieldData, ManagedSerializableFieldDataDictionaryRTTI>
	{
	private:
		ManagedSerializableDictionaryPtr getValue(ManagedSerializableFieldDataDictionary* obj) { return obj->value; }
		void setValue(ManagedSerializableFieldDataDictionary* obj, ManagedSerializableDictionaryPtr val) { obj->value = val; }

	public:
		ManagedSerializableFieldDataDictionaryRTTI()
		{
			addReflectablePtrField("mValue", 0, &ManagedSerializableFieldDataDictionaryRTTI::getValue, &ManagedSerializableFieldDataDictionaryRTTI::setValue);
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
			return bs_shared_ptr<ManagedSerializableFieldDataDictionary>();
		}
	};
}