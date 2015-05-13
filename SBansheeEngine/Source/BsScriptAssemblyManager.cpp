#include "BsScriptAssemblyManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"
#include "BsMonoUtil.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	ScriptAssemblyManager::ScriptAssemblyManager()
		:mBaseTypesInitialized(false), mSerializeObjectAttribute(nullptr), mDontSerializeFieldAttribute(nullptr), 
		mComponentClass(nullptr), mSceneObjectClass(nullptr), mTexture2DClass(nullptr), mSpriteTextureClass(nullptr),
		mSerializeFieldAttribute(nullptr), mHideInInspectorAttribute(nullptr), mSystemArrayClass(nullptr), mSystemGenericListClass(nullptr),
		mSystemGenericDictionaryClass(nullptr), mManagedResourceClass(nullptr), mFontClass(nullptr), mMissingComponentClass(nullptr),
		mPlainTextClass(nullptr), mScriptCodeClass(nullptr), mShaderClass(nullptr), mMaterialClass(nullptr), mTexture3DClass(nullptr),
		mTextureCubeClass(nullptr), mMeshClass(nullptr)
	{

	}

	ScriptAssemblyManager::~ScriptAssemblyManager()
	{

	}

	Vector<String> ScriptAssemblyManager::getScriptAssemblies() const
	{
		Vector<String> initializedAssemblies;
		for (auto& assemblyPair : mAssemblyInfos)
			initializedAssemblies.push_back(assemblyPair.first);

		return initializedAssemblies;
	}

	void ScriptAssemblyManager::loadAssemblyInfo(const String& assemblyName)
	{
		if(!mBaseTypesInitialized)
			initializeBaseTypes();

		// Process all classes and fields
		UINT32 mUniqueTypeId = 1;

		MonoAssembly* curAssembly = MonoManager::instance().getAssembly(assemblyName);
		if(curAssembly == nullptr)
			return;

		std::shared_ptr<ManagedSerializableAssemblyInfo> assemblyInfo = bs_shared_ptr<ManagedSerializableAssemblyInfo>();
		assemblyInfo->mName = assemblyName;

		mAssemblyInfos[assemblyName] = assemblyInfo;

		// Populate class data
		const Vector<MonoClass*>& allClasses = curAssembly->getAllClasses();
		for(auto& curClass : allClasses)
		{
			if((curClass->isSubClassOf(mComponentClass) || curClass->isSubClassOf(mManagedResourceClass) || 
				curClass->hasAttribute(mSerializeObjectAttribute)) && curClass != mComponentClass && curClass != mManagedResourceClass)
			{
				std::shared_ptr<ManagedSerializableTypeInfoObject> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoObject>();
				typeInfo->mTypeNamespace = curClass->getNamespace();
				typeInfo->mTypeName = curClass->getTypeName();

				MonoType* monoType = mono_class_get_type(curClass->_getInternalClass());
				int monoPrimitiveType = mono_type_get_type(monoType);

				if(monoPrimitiveType == MONO_TYPE_VALUETYPE)
					typeInfo->mValueType = true;
				else
					typeInfo->mValueType = false;

				std::shared_ptr<ManagedSerializableObjectInfo> objInfo = bs_shared_ptr<ManagedSerializableObjectInfo>();

				objInfo->mTypeId = mUniqueTypeId++;

				objInfo->mTypeInfo = typeInfo;
				objInfo->mMonoClass = curClass;

				assemblyInfo->mTypeNameToId[objInfo->getFullTypeName()] = objInfo->mTypeId;
				assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;
			}
		}

		// Populate field data
		for(auto& curClassInfo : assemblyInfo->mObjectInfos)
		{
			std::shared_ptr<ManagedSerializableObjectInfo> objInfo = curClassInfo.second;

			UINT32 mUniqueFieldId = 1;
			const Vector<MonoField*>& fields = objInfo->mMonoClass->getAllFields();

			for(auto& field : fields)
			{
				if(field->isStatic())
					continue;

				ManagedSerializableTypeInfoPtr typeInfo = determineType(field->getType());
				if (typeInfo == nullptr)
					continue;

				std::shared_ptr<ManagedSerializableFieldInfo> fieldInfo = bs_shared_ptr<ManagedSerializableFieldInfo>();
				fieldInfo->mFieldId = mUniqueFieldId++;
				fieldInfo->mName = field->getName();
				fieldInfo->mMonoField = field;
				fieldInfo->mTypeInfo = typeInfo;
				
				MonoFieldVisibility visibility = field->getVisibility();
				if (visibility == MonoFieldVisibility::Public)
				{
					if (!field->hasAttribute(mDontSerializeFieldAttribute))
						fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Serializable);

					if (!field->hasAttribute(mHideInInspectorAttribute))
						fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Inspectable);
				}
				else
				{
					if (field->hasAttribute(mSerializeFieldAttribute))
						fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Serializable);
				}

				objInfo->mFieldNameToId[fieldInfo->mName] = fieldInfo->mFieldId;
				objInfo->mFields[fieldInfo->mFieldId] = fieldInfo;
			}
		}

		// Form parent/child connections
		for(auto& curClass : assemblyInfo->mObjectInfos)
		{
			MonoClass* base = curClass.second->mMonoClass->getBaseClass();
			while(base != nullptr)
			{
				std::shared_ptr<ManagedSerializableObjectInfo> baseObjInfo;
				if(getSerializableObjectInfo(base->getNamespace(), base->getTypeName(), baseObjInfo))
				{
					curClass.second->mBaseClass = baseObjInfo;
					baseObjInfo->mDerivedClasses.push_back(curClass.second);

					break;
				}

				base = base->getBaseClass();
			}
		}

		// Finish object info initialization
		for (auto& curClassInfo : assemblyInfo->mObjectInfos)
		{
			std::shared_ptr<ManagedSerializableObjectInfo> objInfo = curClassInfo.second;
			objInfo->initialize();
		}
	}

	void ScriptAssemblyManager::refreshAssemblyInfo()
	{
		for (auto& assemblyInfoEntry : mAssemblyInfos)
			assemblyInfoEntry.second = nullptr;

		clearScriptObjects();
		for (auto& assemblyInfoEntry : mAssemblyInfos)
		{
			loadAssemblyInfo(assemblyInfoEntry.first);
		}
	}

	ManagedSerializableTypeInfoPtr ScriptAssemblyManager::determineType(MonoClass* monoClass)
	{
		if(!mBaseTypesInitialized)
			BS_EXCEPT(InvalidStateException, "Calling determineType without previously initializing base types.");

		MonoType* monoType = mono_class_get_type(monoClass->_getInternalClass());
		int monoPrimitiveType = mono_type_get_type(monoType);
		
		// If enum get the enum base data type
		bool isEnum = mono_class_is_enum(monoClass->_getInternalClass()) == 1;
		if (isEnum)
		{
			MonoType* underlyingType = mono_type_get_underlying_type(monoType);
			monoPrimitiveType = mono_type_get_type(underlyingType);
		}

		//  Determine field type
		switch(monoPrimitiveType)
		{
		case MONO_TYPE_BOOLEAN:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Bool;
				return typeInfo;
			}
		case MONO_TYPE_CHAR:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Char;
				return typeInfo;
			}
		case MONO_TYPE_I1:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I8;
				return typeInfo;
			}
		case MONO_TYPE_U1:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U8;
				return typeInfo;
			}
		case MONO_TYPE_I2:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I16;
				return typeInfo;
			}
		case MONO_TYPE_U2:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U16;
				return typeInfo;
			}
		case MONO_TYPE_I4:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I32;
				return typeInfo;
			}
		case MONO_TYPE_U4:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U32;
				return typeInfo;
			}
		case MONO_TYPE_I8:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I64;
				return typeInfo;
			}
		case MONO_TYPE_U8:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U64;
				return typeInfo;
			}
		case MONO_TYPE_STRING:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::String;
				return typeInfo;
			}
		case MONO_TYPE_R4:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Float;
				return typeInfo;
			}
		case MONO_TYPE_R8:
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Double;
				return typeInfo;
			}
		case MONO_TYPE_CLASS:
			if(monoClass->isSubClassOf(mTexture2DClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Texture2DRef;
				return typeInfo;
			}
			if (monoClass->isSubClassOf(mTexture3DClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Texture3DRef;
				return typeInfo;
			}
			if (monoClass->isSubClassOf(mTextureCubeClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::TextureCubeRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mSpriteTextureClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::SpriteTextureRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mManagedResourceClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::ManagedResourceRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mShaderClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::ShaderRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mMaterialClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::MaterialRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mMeshClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::MeshRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mPlainTextClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::PlainTextRef;
				return typeInfo;
			}
			else if (monoClass->isSubClassOf(mScriptCodeClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::ScriptCodeRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mSceneObjectClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::SceneObjectRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mComponentClass))
			{
				std::shared_ptr<ManagedSerializableTypeInfoPrimitive> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::ComponentRef;
				return typeInfo;
			}
			else
			{
				std::shared_ptr<ManagedSerializableObjectInfo> objInfo;
				if (getSerializableObjectInfo(monoClass->getNamespace(), monoClass->getTypeName(), objInfo))
					return objInfo->mTypeInfo;
			}

			break;
		case MONO_TYPE_VALUETYPE:
			{
				std::shared_ptr<ManagedSerializableObjectInfo> objInfo;
				if (getSerializableObjectInfo(monoClass->getNamespace(), monoClass->getTypeName(), objInfo))
					return objInfo->mTypeInfo;
			}

			break;
		case MONO_TYPE_GENERICINST:
			if(monoClass->getFullName() == mSystemGenericListClass->getFullName()) // Full name is part of CIL spec, so it is just fine to compare like this
			{
				std::shared_ptr<ManagedSerializableTypeInfoList> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoList>();

				MonoProperty& itemProperty = monoClass->getProperty("Item");
				MonoClass* itemClass = itemProperty.getReturnType();

				if(itemClass != nullptr)
					typeInfo->mElementType = determineType(itemClass);

				return typeInfo;
			}
			else if(monoClass->getFullName() == mSystemGenericDictionaryClass->getFullName())
			{
				std::shared_ptr<ManagedSerializableTypeInfoDictionary> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoDictionary>();

				MonoMethod* getEnumerator = monoClass->getMethod("GetEnumerator");
				MonoClass* enumClass = getEnumerator->getReturnType();

				MonoProperty& currentProp = enumClass->getProperty("Current");
				MonoClass* keyValuePair = currentProp.getReturnType();

				MonoProperty& keyProperty = keyValuePair->getProperty("Key");
				MonoProperty& valueProperty = keyValuePair->getProperty("Value");

				MonoClass* keyClass = keyProperty.getReturnType();
				if(keyClass != nullptr)
					typeInfo->mKeyType = determineType(keyClass);

				MonoClass* valueClass = valueProperty.getReturnType();
				if(valueClass != nullptr)
					typeInfo->mValueType = determineType(valueClass);

				return typeInfo;
			}
			break;
		case MONO_TYPE_SZARRAY:
		case MONO_TYPE_ARRAY:
			{
				std::shared_ptr<ManagedSerializableTypeInfoArray> typeInfo = bs_shared_ptr<ManagedSerializableTypeInfoArray>();

				::MonoClass* elementClass = mono_class_get_element_class(monoClass->_getInternalClass());
				if(elementClass != nullptr)
				{
					MonoClass* monoElementClass = MonoManager::instance().findClass(elementClass);
					if(monoElementClass != nullptr)
						typeInfo->mElementType = determineType(monoElementClass);
				}

				typeInfo->mRank = (UINT32)mono_class_get_rank(monoClass->_getInternalClass());

				return typeInfo;
			}
		}

		return nullptr;
	}

	void ScriptAssemblyManager::clearScriptObjects()
	{
		mBaseTypesInitialized = false;

		mSystemArrayClass = nullptr;
		mSystemGenericListClass = nullptr;
		mSystemGenericDictionaryClass = nullptr;

		mSerializeObjectAttribute = nullptr;
		mDontSerializeFieldAttribute = nullptr;

		mComponentClass = nullptr;
		mSceneObjectClass = nullptr;
		mMissingComponentClass = nullptr;

		mManagedResourceClass = nullptr;
		mTexture2DClass = nullptr;
		mTexture3DClass = nullptr;
		mTextureCubeClass = nullptr;
		mSpriteTextureClass = nullptr;
		mShaderClass = nullptr;
		mMaterialClass = nullptr;
		mMeshClass = nullptr;
		mFontClass = nullptr;
		mPlainTextClass = nullptr;
		mScriptCodeClass = nullptr;

		mSerializeFieldAttribute = nullptr;
		mHideInInspectorAttribute = nullptr;
	}

	void ScriptAssemblyManager::initializeBaseTypes()
	{
		// Get necessary classes for detecting needed class & field information
		MonoAssembly* corlib = MonoManager::instance().getAssembly("corlib");
		if(corlib == nullptr)
			BS_EXCEPT(InvalidStateException, "corlib assembly is not loaded.");

		MonoAssembly* bansheeEngineAssembly = MonoManager::instance().getAssembly(ENGINE_ASSEMBLY);
		if(bansheeEngineAssembly == nullptr)
			BS_EXCEPT(InvalidStateException, String(ENGINE_ASSEMBLY) +  " assembly is not loaded.");

		mSystemArrayClass = corlib->getClass("System", "Array");
		if(mSystemArrayClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find System.Array managed class.");

		mSystemGenericListClass = corlib->getClass("System.Collections.Generic", "List`1");
		if(mSystemGenericListClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find List<T> managed class.");

		mSystemGenericDictionaryClass = corlib->getClass("System.Collections.Generic", "Dictionary`2");
		if(mSystemGenericDictionaryClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Dictionary<TKey, TValue> managed class.");

		mSerializeObjectAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "SerializeObject");
		if(mSerializeObjectAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find SerializableObject managed class.");

		mDontSerializeFieldAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "DontSerializeField");
		if(mDontSerializeFieldAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find DontSerializeField managed class.");

		mComponentClass = bansheeEngineAssembly->getClass("BansheeEngine", "Component");
		if(mComponentClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Component managed class.");

		mMissingComponentClass = bansheeEngineAssembly->getClass("BansheeEngine", "MissingComponent");
		if (mMissingComponentClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find MissingComponent managed class.");

		mSceneObjectClass = bansheeEngineAssembly->getClass("BansheeEngine", "SceneObject");
		if(mSceneObjectClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find SceneObject managed class.");

		mManagedResourceClass = bansheeEngineAssembly->getClass("BansheeEngine", "ManagedResource");
		if (mManagedResourceClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find ManagedResource managed class.");

		mTexture2DClass = bansheeEngineAssembly->getClass("BansheeEngine", "Texture2D");
		if(mTexture2DClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Texture2D managed class.");

		mTexture3DClass = bansheeEngineAssembly->getClass("BansheeEngine", "Texture3D");
		if (mTexture3DClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Texture3D managed class.");

		mTextureCubeClass = bansheeEngineAssembly->getClass("BansheeEngine", "TextureCube");
		if (mTextureCubeClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find TextureCube managed class.");

		mSpriteTextureClass = bansheeEngineAssembly->getClass("BansheeEngine", "SpriteTexture");
		if(mSpriteTextureClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find SpriteTexture managed class.");

		mShaderClass = bansheeEngineAssembly->getClass("BansheeEngine", "Shader");
		if (mShaderClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Shader managed class.");

		mMaterialClass = bansheeEngineAssembly->getClass("BansheeEngine", "Material");
		if (mMaterialClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Material managed class.");

		mMeshClass = bansheeEngineAssembly->getClass("BansheeEngine", "Mesh");
		if (mMeshClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Mesh managed class.");

		mFontClass = bansheeEngineAssembly->getClass("BansheeEngine", "Font");
		if (mFontClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Font managed class.");

		mPlainTextClass = bansheeEngineAssembly->getClass("BansheeEngine", "PlainText");
		if (mPlainTextClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find PlainText managed class.");

		mScriptCodeClass = bansheeEngineAssembly->getClass("BansheeEngine", "ScriptCode");
		if (mScriptCodeClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find ScriptCode managed class.");

		mSerializeFieldAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "SerializeField");
		if(mSerializeFieldAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find SerializeField managed class.");

		mHideInInspectorAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "HideInInspector");
		if(mHideInInspectorAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find HideInInspector managed class.");

		mBaseTypesInitialized = true;
	}

	bool ScriptAssemblyManager::getSerializableObjectInfo(const String& ns, const String& typeName, std::shared_ptr<ManagedSerializableObjectInfo>& outInfo)
	{
		String fullName = ns + "." + typeName;
		for(auto& curAssembly : mAssemblyInfos)
		{
			if (curAssembly.second == nullptr)
				continue;

			auto iterFind = curAssembly.second->mTypeNameToId.find(fullName);
			if(iterFind != curAssembly.second->mTypeNameToId.end())
			{
				outInfo = curAssembly.second->mObjectInfos[iterFind->second];

				return true;
			}
		}

		return false;
	}

	bool ScriptAssemblyManager::hasSerializableObjectInfo(const String& ns, const String& typeName)
	{
		String fullName = ns + "." + typeName;
		for(auto& curAssembly : mAssemblyInfos)
		{
			auto iterFind = curAssembly.second->mTypeNameToId.find(fullName);
			if(iterFind != curAssembly.second->mTypeNameToId.end())
				return true;
		}

		return false;
	}
}