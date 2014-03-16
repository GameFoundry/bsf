#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"
#include <mono/jit/jit.h>

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
		SerializableObjectValue,
		SerializableObjectRef,
		TextureRef,
		SpriteTextureRef,
		SceneObjectRef,
		ComponentRef,
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

		bool isArray();
		bool isReferenceType();
		bool isNull(MonoObject* obj);
		void setNull(MonoObject* obj);

		CM::UINT32 getNumArrayElements(MonoObject* obj);
		void setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, bool discardExisting = true);

		void setU8(MonoObject* obj, CM::UINT8 val, CM::UINT32 arrayIdx = 0);
		CM::UINT8 getU8(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setI8(MonoObject* obj, CM::INT8 val, CM::UINT32 arrayIdx = 0);
		CM::INT8 getI8(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setU16(MonoObject* obj, CM::UINT16 val, CM::UINT32 arrayIdx = 0);
		CM::UINT16 getU16(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setI16(MonoObject* obj, CM::INT16 val, CM::UINT32 arrayIdx = 0);
		CM::INT16 getI16(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setU32(MonoObject* obj, CM::UINT32 val, CM::UINT32 arrayIdx = 0);
		CM::UINT32 getU32(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setI32(MonoObject* obj, CM::INT32 val, CM::UINT32 arrayIdx = 0);
		CM::INT32 getI32(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setU64(MonoObject* obj, CM::UINT64 val, CM::UINT32 arrayIdx = 0);
		CM::UINT64 getU64(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setI64(MonoObject* obj, CM::INT64 val, CM::UINT32 arrayIdx = 0);
		CM::INT64 getI64(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setBool(MonoObject* obj, bool val, CM::UINT32 arrayIdx = 0);
		bool getBool(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setChar(MonoObject* obj, wchar_t val, CM::UINT32 arrayIdx = 0);
		wchar_t getChar(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setFloat(MonoObject* obj, float val, CM::UINT32 arrayIdx = 0);
		float getFloat(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setDouble(MonoObject* obj, double val, CM::UINT32 arrayIdx = 0);
		double getDouble(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setString(MonoObject* obj, const CM::WString& val, CM::UINT32 arrayIdx = 0);
		CM::WString getString(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setTexture(MonoObject* obj, const CM::HTexture& resource, CM::UINT32 arrayIdx = 0);
		CM::HTexture getTexture(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setSpriteTexture(MonoObject* obj, const HSpriteTexture& resource, CM::UINT32 arrayIdx = 0);
		HSpriteTexture getSpriteTexture(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setSceneObject(MonoObject* obj, const CM::HSceneObject& sceneObject, CM::UINT32 arrayIdx = 0);
		CM::HSceneObject getSceneObject(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setComponent(MonoObject* obj, const CM::HComponent& component, CM::UINT32 arrayIdx = 0);
		CM::HComponent getComponent(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		void setSerializableObject(MonoObject* obj, const MonoObject* value, CM::UINT32 arrayIdx = 0);
		MonoObject* getSerializableObject(MonoObject* obj, CM::UINT32 arrayIdx = 0);

	private:
		void setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx = 0);
		void* getValue(MonoObject* obj, CM::UINT32 arrayIdx = 0);
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
		bool hasSerializableObjectInfo(const CM::String& ns, const CM::String& typeName);
	private:
		CM::UnorderedMap<CM::String, std::shared_ptr<SerializableAssemblyInfo>>::type mAssemblyInfos;

		void clearScriptObjects(const CM::String& assemblyName);
	};
}