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

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfo : public IReflectable
	{
	public:
		virtual ~ScriptSerializableTypeInfo() {}

		virtual bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const = 0;
		virtual bool isTypeLoaded() const = 0;

		virtual ::MonoClass* getMonoClass() const = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoPrimitive : public ScriptSerializableTypeInfo
	{
	public:
		ScriptPrimitiveType mType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoPrimitiveRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoObject : public ScriptSerializableTypeInfo
	{
	public:
		String mTypeNamespace;
		String mTypeName;
		bool mValueType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoArray : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mElementType;
		UINT32 mRank;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoList : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mElementType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableTypeInfoDictionary : public ScriptSerializableTypeInfo
	{
	public:
		ScriptSerializableTypeInfoPtr mKeyType;
		ScriptSerializableTypeInfoPtr mValueType;

		bool matches(const ScriptSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableTypeInfoDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableFieldInfo : public IReflectable
	{
	public:
		ScriptSerializableFieldInfo();

		String mName;
		UINT32 mFieldId;

		ScriptSerializableTypeInfoPtr mTypeInfo;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		bool isSerializable() const { return ((UINT32)mFlags & (UINT32)ScriptFieldFlags::Serializable) != 0; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableFieldInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableObjectInfo : public IReflectable
	{
	public:
		ScriptSerializableObjectInfo();

		ScriptSerializableTypeInfoObjectPtr mTypeInfo;
		UINT32 mTypeId;

		MonoClass* mMonoClass;

		UnorderedMap<String, UINT32>::type mFieldNameToId;
		UnorderedMap<UINT32, std::shared_ptr<ScriptSerializableFieldInfo>>::type mFields;

		std::shared_ptr<ScriptSerializableObjectInfo> mBaseClass;
		Vector<std::weak_ptr<ScriptSerializableObjectInfo>>::type mDerivedClasses;

		String getFullTypeName() const { return mTypeInfo->mTypeNamespace + "." + mTypeInfo->mTypeName; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableObjectInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ScriptSerializableAssemblyInfo : public IReflectable
	{
	public:
		String mName;

		UnorderedMap<String, UINT32>::type mTypeNameToId;
		UnorderedMap<UINT32, std::shared_ptr<ScriptSerializableObjectInfo>>::type mObjectInfos;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ScriptSerializableAssemblyInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}