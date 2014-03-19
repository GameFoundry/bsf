#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
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
		Inspectable = 0x02
	};

	struct BS_SCR_BE_EXPORT ScriptSerializableFieldInfo : public CM::IReflectable
	{
		ScriptSerializableFieldInfo();
		virtual ~ScriptSerializableFieldInfo() { }

		CM::String mName;
		CM::String mTypeNamespace;
		CM::String mTypeName;
		CM::UINT32 mFieldId;

		ScriptFieldType mType;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		virtual bool isArray() const = 0;
		virtual bool isReferenceType() const = 0;

		bool isNull(MonoObject* obj);
		void setNull(MonoObject* obj);

		virtual CM::UINT32 getNumArrayElements(MonoObject* obj, CM::UINT32 dimension) = 0;
		virtual void setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, CM::UINT32 dimension, bool discardExisting = true) = 0;

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

	protected:
		virtual void setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx = 0) = 0;
		virtual void* getValue(MonoObject* obj, CM::UINT32 arrayIdx = 0) = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableFieldInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	struct BS_SCR_BE_EXPORT ScriptSerializableFieldInfoPlain : public ScriptSerializableFieldInfo
	{
	public:
		virtual bool isArray() const { return false; }
		virtual bool isReferenceType() const;

		virtual CM::UINT32 getNumArrayElements(MonoObject* obj, CM::UINT32 dimension);
		virtual void setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, CM::UINT32 dimension, bool discardExisting = true);

	protected:
		virtual void setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx = 0);
		virtual void* getValue(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableFieldInfoPlainRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	struct BS_SCR_BE_EXPORT ScriptSerializableFieldInfoArray : public ScriptSerializableFieldInfo
	{
	public:
		ScriptSerializableFieldInfoArray();

		CM::UINT8 mArrayDimensions;

		virtual bool isArray() const { return true; }
		virtual bool isReferenceType() const { return true; }

		virtual CM::UINT32 getNumArrayElements(MonoObject* obj, CM::UINT32 dimension);
		virtual void setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, CM::UINT32 dimension, bool discardExisting = true);

	protected:
		virtual void setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx = 0);
		virtual void* getValue(MonoObject* obj, CM::UINT32 arrayIdx = 0);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableFieldInfoArrayRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	struct BS_SCR_BE_EXPORT ScriptSerializableObjectInfo : public CM::IReflectable
	{
		ScriptSerializableObjectInfo();

		CM::String mNamespace;
		CM::String mTypeName;
		CM::UINT32 mTypeId;

		MonoClass* mMonoClass;

		CM::UnorderedMap<CM::String, CM::UINT32>::type mFieldNameToId;
		CM::UnorderedMap<CM::UINT32, std::shared_ptr<ScriptSerializableFieldInfo>>::type mFields;

		std::weak_ptr<ScriptSerializableObjectInfo> mBaseClass;
		CM::Vector<std::weak_ptr<ScriptSerializableObjectInfo>>::type mDerivedClasses;

		CM::String getFullTypeName() const { return mNamespace + "." + mTypeName; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableObjectInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	struct BS_SCR_BE_EXPORT ScriptSerializableAssemblyInfo : public CM::IReflectable
	{
		CM::String mName;

		CM::UnorderedMap<CM::String, CM::UINT32>::type mTypeNameToId;
		CM::UnorderedMap<CM::UINT32, std::shared_ptr<ScriptSerializableObjectInfo>>::type mObjectInfos;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableAssemblyInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};
}