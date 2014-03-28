#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmIReflectable.h"
#include <mono/jit/jit.h>

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
		Float,
		Double,
		String,
		TextureRef,
		SpriteTextureRef,
		SceneObjectRef,
		ComponentRef
	};

	enum class ScriptFieldFlags
	{
		Serializable = 0x01,
		Inspectable = 0x02
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfo : public CM::IReflectable
	{
	public:
		virtual ~ScriptSerializableTypeInfo() {}

		virtual bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const = 0;
		virtual bool isTypeLoaded() const = 0;
		virtual bool isMonoObject() const = 0;
		virtual ::MonoClass* getMonoClass() const = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoPrimitive : public ScriptSerializableTypeInfo
	{
	public:
		ScriptPrimitiveType mType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;
		bool isMonoObject() const;
		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoPrimitiveRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoObject : public ScriptSerializableTypeInfo
	{
	public:
		CM::String mTypeNamespace;
		CM::String mTypeName;
		bool mValueType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;
		bool isMonoObject() const { return true; }
		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoObjectRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoArray : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mElementType;
		CM::UINT32 mRank;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;
		bool isMonoObject() const { return true; }
		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoArrayRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoList : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mElementType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;
		bool isMonoObject() const { return true; }
		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoListRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoDictionary : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mKeyType;
		ScriptSerializableTypeInfoPtr mValueType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;
		bool isMonoObject() const { return true; }
		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoDictionaryRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfo : public CM::IReflectable
	{
	public:
		ScriptSerializableFieldInfo();

		CM::String mName;
		CM::UINT32 mFieldId;

		ScriptSerializableTypeInfoPtr mTypeInfo;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		bool isSerializable() const { return ((CM::UINT32)mFlags & (CM::UINT32)ScriptFieldFlags::Serializable) != 0; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableFieldInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableObjectInfo : public CM::IReflectable
	{
	public:
		ScriptSerializableObjectInfo();

		ScriptSerializableTypeInfoObjectPtr mTypeInfo;
		CM::UINT32 mTypeId;

		MonoClass* mMonoClass;

		CM::UnorderedMap<CM::String, CM::UINT32>::type mFieldNameToId;
		CM::UnorderedMap<CM::UINT32, std::shared_ptr<ScriptSerializableFieldInfo>>::type mFields;

		std::shared_ptr<ScriptSerializableObjectInfo> mBaseClass;
		CM::Vector<std::weak_ptr<ScriptSerializableObjectInfo>>::type mDerivedClasses;

		CM::String getFullTypeName() const { return mTypeInfo->mTypeNamespace + "." + mTypeInfo->mTypeName; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableObjectInfoRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableAssemblyInfo : public CM::IReflectable
	{
	public:
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