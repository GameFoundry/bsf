//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Valid serializable script types.
	 */
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
		Texture2DRef,
		Texture3DRef,
		TextureCubeRef,
		SpriteTextureRef,
		ManagedResourceRef,
		PlainTextRef,
		ScriptCodeRef,
		ShaderRef,
		ShaderIncludeRef,
		MaterialRef,
		MeshRef,
		PrefabRef,
		FontRef,
		StringTableRef,
		GUISkinRef,
		SceneObjectRef,
		ComponentRef,
		PhysicsMaterialRef,
		PhysicsMeshRef,
		Count // Keep at end
	};

	/**
	 * @brief	Flags that are used to further define
	 *			a field in a managed serializable object.
	 */
	enum class ScriptFieldFlags
	{
		Serializable = 0x01,
		Inspectable = 0x02
	};

	/**
	 * @brief	Contains information about a type of a managed serializable object.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfo : public IReflectable
	{
	public:
		virtual ~ManagedSerializableTypeInfo() {}

		/**
		 * @brief	Checks if the current type matches the provided type.
		 */
		virtual bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const = 0;

		/**
		 * @brief	Checks does the managed type this object represents still exists.
		 *
		 * @note	e.g. If assemblies get refreshed user could have renamed or removed
		 *			some types.
		 */
		virtual bool isTypeLoaded() const = 0;

		/**
		 * @brief	Returns the internal managed class of the type this object represents.
		 *			Returns null if the type doesn't exist.
		 */
		virtual ::MonoClass* getMonoClass() const = 0;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about a type of a managed serializable primitive (e.g. int, float, etc.).
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoPrimitive : public ManagedSerializableTypeInfo
	{
	public:
		/**
		 * @copydoc	ManagedSerializableTypeInfo::matches
		 */
		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::isTypeLoaded
		 */
		bool isTypeLoaded() const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::getMonoClass
		 */
		::MonoClass* getMonoClass() const override;

		ScriptPrimitiveType mType;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoPrimitiveRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about a type of a managed serializable complex object (e.g. struct or class).
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoObject : public ManagedSerializableTypeInfo
	{
	public:
		/**
		 * @copydoc	ManagedSerializableTypeInfo::matches
		 */
		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::isTypeLoaded
		 */
		bool isTypeLoaded() const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::getMonoClass
		 */
		::MonoClass* getMonoClass() const override;

		String mTypeNamespace;
		String mTypeName;
		bool mValueType;
		UINT32 mTypeId;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoObjectRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about a type of a managed serializable Array.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoArray : public ManagedSerializableTypeInfo
	{
	public:
		/**
		 * @copydoc	ManagedSerializableTypeInfo::matches
		 */
		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::isTypeLoaded
		 */
		bool isTypeLoaded() const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::getMonoClass
		 */
		::MonoClass* getMonoClass() const override;

		ManagedSerializableTypeInfoPtr mElementType;
		UINT32 mRank;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoArrayRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about a type of a managed serializable List.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoList : public ManagedSerializableTypeInfo
	{
	public:
		/**
		 * @copydoc	ManagedSerializableTypeInfo::matches
		 */
		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::isTypeLoaded
		 */
		bool isTypeLoaded() const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::getMonoClass
		 */
		::MonoClass* getMonoClass() const override;

		ManagedSerializableTypeInfoPtr mElementType;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoListRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about a type of a managed serializable Dictionary.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableTypeInfoDictionary : public ManagedSerializableTypeInfo
	{
	public:
		/**
		 * @copydoc	ManagedSerializableTypeInfo::matches
		 */
		bool matches(const ManagedSerializableTypeInfoPtr& typeInfo) const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::isTypeLoaded
		 */
		bool isTypeLoaded() const override;

		/**
		 * @copydoc	ManagedSerializableTypeInfo::getMonoClass
		 */
		::MonoClass* getMonoClass() const override;

		ManagedSerializableTypeInfoPtr mKeyType;
		ManagedSerializableTypeInfoPtr mValueType;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableTypeInfoDictionaryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains data about a single field in a managed complex object.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableFieldInfo : public IReflectable
	{
	public:
		ManagedSerializableFieldInfo();

		/**
		 * @brief	Determines should the field be serialized when serializing the parent object.
		 */
		bool isSerializable() const { return ((UINT32)mFlags & (UINT32)ScriptFieldFlags::Serializable) != 0; }

		String mName;
		UINT32 mFieldId;
		UINT32 mParentTypeId;

		ManagedSerializableTypeInfoPtr mTypeInfo;
		ScriptFieldFlags mFlags;

		MonoField* mMonoField;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableFieldInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains data about fields of a complex object, and the object's
	 *			class hierarchy if it belongs to one.
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableObjectInfo : public IReflectable
	{
	public:
		ManagedSerializableObjectInfo();

		/**
		 * @brief	Returns the managed type name of the object's type, including the namespace in format
		 *			"namespace.typename".
		 */
		String getFullTypeName() const { return mTypeInfo->mTypeNamespace + "." + mTypeInfo->mTypeName; }

		/**
		 * @brief	Attempts to find a field part of this object that matches the provided parameters.
		 *
		 * @param	fieldInfo		Object describing the managed field. Normally this will be a field
		 *							that was deserialized and you need to ensure it still exists in
		 *							its parent type, while retrieving the new field info.
		 * @param	fieldTypeInfo	Type information about the type containing the object. Used for
		 *							debug purposes to ensure the current object's type matches.
		 *
		 * @return	Found field info within this object, or null if not found.
		 */
		ManagedSerializableFieldInfoPtr findMatchingField(const ManagedSerializableFieldInfoPtr& fieldInfo,
			const ManagedSerializableTypeInfoPtr& fieldTypeInfo) const;

		ManagedSerializableTypeInfoObjectPtr mTypeInfo;
		MonoClass* mMonoClass;

		UnorderedMap<String, UINT32> mFieldNameToId;
		UnorderedMap<UINT32, std::shared_ptr<ManagedSerializableFieldInfo>> mFields;

		std::shared_ptr<ManagedSerializableObjectInfo> mBaseClass;
		Vector<std::weak_ptr<ManagedSerializableObjectInfo>> mDerivedClasses;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableObjectInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains information about all managed serializable objects in a specific managed assembly.
	 */
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
		virtual RTTITypeBase* getRTTI() const override;
	};
}