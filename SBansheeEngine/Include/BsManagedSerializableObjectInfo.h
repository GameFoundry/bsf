#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
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
		ManagedResourceRef,
		SceneObjectRef,
		ComponentRef
	};

	enum class ScriptFieldFlags
	{
		Serializable = 0x01,
		Inspectable = 0x02
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfo : public IReflectable
	{
	public:
		virtual ~ManagedSerializableTypeInfo() {}

		virtual bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const = 0;
		virtual bool isTypeLoaded() const = 0;

		virtual ::MonoClass* getMonoClass() const = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoPrimitive : public ManagedSerializableTypeInfo
	{
	public:
		ScriptPrimitiveType mType;

		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoPrimitiveRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoObject : public ManagedSerializableTypeInfo
	{
	public:
		String mTypeNamespace;
		String mTypeName;
		bool mValueType;

		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoArray : public ManagedSerializableTypeInfo
	{
	public:
		ManagedSerializableTypeInfoPtr mElementType;
		UINT32 mRank;

		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoList : public ManagedSerializableTypeInfo
	{
	public:
		ManagedSerializableTypeInfoPtr mElementType;

		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoDictionary : public ManagedSerializableTypeInfo
	{
	public:
		ManagedSerializableTypeInfoPtr mKeyType;
		ManagedSerializableTypeInfoPtr mValueType;

		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const;
		bool isTypeLoaded() const;

		::MonoClass* getMonoClass() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableFieldInfo : public IReflectable
	{
	public:
		ManagedSerializableFieldInfo();

		String mName;
		UINT32 mFieldId;

		ManagedSerializableTypeInfoPtr mTypeInfo;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		bool isSerializable() const { return ((UINT32)mFlags & (UINT32)ScriptFieldFlags::Serializable) != 0; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableFieldInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableObjectInfo : public IReflectable
	{
	public:
		ManagedSerializableObjectInfo();

		ManagedSerializableTypeInfoObjectPtr mTypeInfo;
		UINT32 mTypeId;

		MonoClass* mMonoClass;

		UnorderedMap<String, UINT32> mFieldNameToId;
		UnorderedMap<UINT32, std::shared_ptr<ManagedSerializableFieldInfo>> mFields;

		std::shared_ptr<ManagedSerializableObjectInfo> mBaseClass;
		Vector<std::weak_ptr<ManagedSerializableObjectInfo>> mDerivedClasses;

		String getFullTypeName() const { return mTypeInfo->mTypeNamespace + "." + mTypeInfo->mTypeName; }

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableObjectInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_SCR_BE_EXPORT ManagedSerializableAssemblyInfo : public IReflectable
	{
	public:
		String mName;

		UnorderedMap<String, UINT32> mTypeNameToId;
		UnorderedMap<UINT32, std::shared_ptr<ManagedSerializableObjectInfo>> mObjectInfos;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableAssemblyInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}