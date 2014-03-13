#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	enum class ScriptFieldType
	{
		Bool,
		Char,
		I8,
		U8,
		I16,
		U16,
		I32,
		U32,
		I64,
		U64,
		Float,
		Double,
		String,
		SerializableObject,
		ResourceRef,
		GameObjectRef,
		Other
	};

	enum class ScriptFieldFlags
	{
		Serializable = 0x01,
		Array = 0x02,
		Inspectable = 0x04
	};

	struct BS_SCR_BE_EXPORT SerializableFieldInfo
	{
		SerializableFieldInfo();
		~SerializableFieldInfo();

		CM::String mName;
		CM::String mTypeNamespace;
		CM::String mTypeName;
		CM::UINT32 mFieldId;

		ScriptFieldType mType;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		CM::UINT32 getNumArrayElement(MonoObject* obj);
		void setNumArrayElements(MonoObject* obj, CM::UINT32 numElements);

		void setU8(MonoObject* obj, CM::UINT8 val, CM::UINT32 arrayIdx = 0);
		CM::UINT8 getU8(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setI8(MonoObject* obj,CM::INT8 val, CM::UINT32 arrayIdx = 0);
		CM::INT8 getI8(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setU16(MonoObject* obj,CM::UINT16 val, CM::UINT32 arrayIdx = 0);
		CM::UINT16 getU16(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setI16(MonoObject* obj,CM::INT16 val, CM::UINT32 arrayIdx = 0);
		CM::INT16 getI16(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setU32(MonoObject* obj,CM::UINT32 val, CM::UINT32 arrayIdx = 0);
		CM::UINT32 getU32(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setI32(MonoObject* obj,CM::INT32 val, CM::UINT32 arrayIdx = 0);
		CM::INT32 getI32(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setU64(MonoObject* obj,CM::UINT64 val, CM::UINT32 arrayIdx = 0);
		CM::UINT64 getU64(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setI64(MonoObject* obj,CM::INT64 val, CM::UINT32 arrayIdx = 0);
		CM::INT64 getI64(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setBool(MonoObject* obj,bool val, CM::UINT32 arrayIdx = 0);
		bool getBool(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setChar(MonoObject* obj,wchar_t val, CM::UINT32 arrayIdx = 0);
		wchar_t getChar(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setFloat(MonoObject* obj,float val, CM::UINT32 arrayIdx = 0);
		float getFloat(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setDouble(MonoObject* obj,double val, CM::UINT32 arrayIdx = 0);
		double getDouble(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setString(MonoObject* obj,const CM::WString& val, CM::UINT32 arrayIdx = 0);
		CM::WString getString(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setResource(MonoObject* obj,const CM::HResource& resource, CM::UINT32 arrayIdx = 0);
		CM::HResource getResource(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		void setGameObject(MonoObject* obj,const CM::HGameObject& gameObject, CM::UINT32 arrayIdx = 0);
		CM::HGameObject getGameObject(MonoObject* obj,CM::UINT32 arrayIdx = 0);

		// TODO - Set/Get for SerializableObject
	};

	struct BS_SCR_BE_EXPORT SerializableObjectInfo
	{
		SerializableObjectInfo();
		~SerializableObjectInfo();

		CM::String mNamespace;
		CM::String mTypeName;
		CM::UINT32 mTypeId;

		MonoClass* mMonoClass;

		CM::UnorderedMap<CM::String, CM::UINT32>::type mFieldNameToId;
		CM::UnorderedMap<CM::UINT32, SerializableFieldInfo*>::type mFields;

		std::weak_ptr<SerializableObjectInfo> mBaseClass;
		CM::Vector<std::weak_ptr<SerializableObjectInfo>>::type mDerivedClasses;
	};

	struct BS_SCR_BE_EXPORT SerializableAssemblyInfo
	{
		CM::UnorderedMap<CM::String, CM::UINT32>::type mTypeNameToId;
		CM::UnorderedMap<CM::UINT32, std::shared_ptr<SerializableObjectInfo>>::type mObjectInfos;
	};

	class BS_SCR_BE_EXPORT RuntimeScriptObjects : public CM::Module<RuntimeScriptObjects>
	{
	public:
		~RuntimeScriptObjects();

		void refreshScriptObjects(const CM::String& assemblyName);
		bool getSerializableObjectInfo(const CM::String& ns, const CM::String& typeName, std::shared_ptr<SerializableObjectInfo>& outInfo);
	private:
		CM::UnorderedMap<CM::String, std::shared_ptr<SerializableAssemblyInfo>>::type mAssemblyInfos;

		void clearScriptObjects(const CM::String& assemblyName);
	};
}