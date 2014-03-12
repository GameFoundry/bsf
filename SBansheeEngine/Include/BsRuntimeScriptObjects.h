#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	enum class ScriptPrimitiveType
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
		String
	};

	enum class ScriptFieldType
	{
		Primitive,
		Complex,
		ResourceRef,
		GameObjectRef
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

		ScriptFieldType mType;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;
	};

	struct BS_SCR_BE_EXPORT SerializableObjectInfo
	{
		SerializableObjectInfo();
		~SerializableObjectInfo();

		CM::String mNamespace;
		CM::String mTypeName;

		MonoClass* mMonoClass;

		CM::UnorderedMap<CM::String, SerializableFieldInfo*>::type mFields;
	};

	class BS_SCR_BE_EXPORT RuntimeScriptObjects : public CM::Module<RuntimeScriptObjects>
	{
	public:
		~RuntimeScriptObjects();

		void refreshScriptObjects();

	private:
		CM::UnorderedMap<CM::String, SerializableObjectInfo*>::type mObjectInfos;

		void clearScriptObjects();
	};
}