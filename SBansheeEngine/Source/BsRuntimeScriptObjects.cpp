#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptSerializableObjectInfo.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"
#include "BsMonoUtil.h"
#include "CmRTTIType.h"

namespace BansheeEngine
{
	RuntimeScriptObjects::RuntimeScriptObjects()
		:mBaseTypesInitialized(false), mSerializableObjectAttribute(nullptr), mDontSerializeFieldAttribute(nullptr), 
		mComponentClass(nullptr), mSceneObjectClass(nullptr), mTextureClass(nullptr), mSpriteTextureClass(nullptr),
		mSerializeFieldAttribute(nullptr), mHideInInspectorAttribute(nullptr), mSystemArrayClass(nullptr), mSystemGenericListClass(nullptr),
		mSystemGenericDictionaryClass(nullptr)
	{

	}

	RuntimeScriptObjects::~RuntimeScriptObjects()
	{

	}

	void RuntimeScriptObjects::refreshScriptObjects(const String& assemblyName)
	{
		clearScriptObjects(assemblyName);

		if(!mBaseTypesInitialized)
			initializeBaseTypes();

		// Process all classes and fields
		UINT32 mUniqueTypeId = 1;

		MonoAssembly* curAssembly = MonoManager::instance().getAssembly(assemblyName);
		if(curAssembly == nullptr)
			return;

		std::shared_ptr<ScriptSerializableAssemblyInfo> assemblyInfo = cm_shared_ptr<ScriptSerializableAssemblyInfo>();
		assemblyInfo->mName = assemblyName;

		mAssemblyInfos[assemblyName] = assemblyInfo;

		// Populate class data
		const Vector<MonoClass*>::type& allClasses = curAssembly->getAllClasses();
		for(auto& curClass : allClasses)
		{
			if((curClass->isSubClassOf(mComponentClass) || curClass->hasAttribute(mSerializableObjectAttribute)) && curClass != mComponentClass)
			{
				std::shared_ptr<ScriptSerializableTypeInfoObject> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoObject>();
				typeInfo->mTypeNamespace = curClass->getNamespace();
				typeInfo->mTypeName = curClass->getTypeName();

				MonoType* monoType = mono_class_get_type(curClass->_getInternalClass());
				int monoPrimitiveType = mono_type_get_type(monoType);

				if(monoPrimitiveType == MONO_TYPE_VALUETYPE)
					typeInfo->mValueType = true;
				else
					typeInfo->mValueType = false;

				std::shared_ptr<ScriptSerializableObjectInfo> objInfo = cm_shared_ptr<ScriptSerializableObjectInfo>();

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
			std::shared_ptr<ScriptSerializableObjectInfo> objInfo = curClassInfo.second;

			String fullTypeName = objInfo->getFullTypeName();
			assemblyInfo->mTypeNameToId[fullTypeName] = objInfo->mTypeId;
			assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;

			UINT32 mUniqueFieldId = 1;
			const Vector<MonoField*>::type& fields = objInfo->mMonoClass->getAllFields();

			for(auto& field : fields)
			{
				if(field->isStatic())
					continue;

				std::shared_ptr<ScriptSerializableFieldInfo> fieldInfo = cm_shared_ptr<ScriptSerializableFieldInfo>();

				fieldInfo->mFieldId = mUniqueFieldId++;
				fieldInfo->mName = field->getName();
				fieldInfo->mMonoField = field;
				fieldInfo->mTypeInfo = determineType(field->getType());
				
				if(fieldInfo->mTypeInfo != nullptr)
				{
					MonoFieldVisibility visibility = field->getVisibility();
					if(visibility == MonoFieldVisibility::Public)
					{
						if(!field->hasAttribute(mDontSerializeFieldAttribute))
							fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Serializable);

						if(!field->hasAttribute(mHideInInspectorAttribute))
							fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Inspectable);
					}
					else
					{
						if(field->hasAttribute(mSerializeFieldAttribute))
							fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Serializable);
					}
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
				std::shared_ptr<ScriptSerializableObjectInfo> baseObjInfo;
				if(getSerializableObjectInfo(base->getNamespace(), base->getTypeName(), baseObjInfo))
				{
					curClass.second->mBaseClass = baseObjInfo;
					baseObjInfo->mDerivedClasses.push_back(curClass.second);

					break;
				}

				base = base->getBaseClass();
			}
		}
	}

	ScriptSerializableTypeInfoPtr RuntimeScriptObjects::determineType(MonoClass* monoClass)
	{
		if(!mBaseTypesInitialized)
			CM_EXCEPT(InvalidStateException, "Calling determineType without previously initializing base types.");

		MonoType* monoType = mono_class_get_type(monoClass->_getInternalClass());
		int monoPrimitiveType = mono_type_get_type(monoType);

		//  Determine field type
		switch(monoPrimitiveType)
		{
		case MONO_TYPE_BOOLEAN:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Bool;
				return typeInfo;
			}
		case MONO_TYPE_CHAR:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Char;
				return typeInfo;
			}
		case MONO_TYPE_I1:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I8;
				return typeInfo;
			}
		case MONO_TYPE_U1:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U8;
				return typeInfo;
			}
		case MONO_TYPE_I2:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I16;
				return typeInfo;
			}
		case MONO_TYPE_U2:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U16;
				return typeInfo;
			}
		case MONO_TYPE_I4:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I32;
				return typeInfo;
			}
		case MONO_TYPE_U4:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U32;
				return typeInfo;
			}
		case MONO_TYPE_I8:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::I64;
				return typeInfo;
			}
		case MONO_TYPE_U8:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::U64;
				return typeInfo;
			}
		case MONO_TYPE_STRING:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::String;
				return typeInfo;
			}
		case MONO_TYPE_R4:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Float;
				return typeInfo;
			}
		case MONO_TYPE_R8:
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::Double;
				return typeInfo;
			}
		case MONO_TYPE_CLASS:
			if(monoClass->isSubClassOf(mTextureClass))
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::TextureRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mSpriteTextureClass))
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::SpriteTextureRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mSceneObjectClass))
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::SceneObjectRef;
				return typeInfo;
			}
			else if(monoClass->isSubClassOf(mComponentClass))
			{
				std::shared_ptr<ScriptSerializableTypeInfoPrimitive> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoPrimitive>();
				typeInfo->mType = ScriptPrimitiveType::ComponentRef;
				return typeInfo;
			}
			else
			{
				if(hasSerializableObjectInfo(monoClass->getNamespace(), monoClass->getTypeName()))
				{
					std::shared_ptr<ScriptSerializableTypeInfoObject> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoObject>();
					typeInfo->mTypeNamespace = monoClass->getNamespace();
					typeInfo->mTypeName = monoClass->getTypeName();
					typeInfo->mValueType = false;

					return typeInfo;
				}
			}

			break;
		case MONO_TYPE_VALUETYPE:
			if(hasSerializableObjectInfo(monoClass->getNamespace(), monoClass->getTypeName()))
			{
				std::shared_ptr<ScriptSerializableTypeInfoObject> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoObject>();
				typeInfo->mTypeNamespace = monoClass->getNamespace();
				typeInfo->mTypeName = monoClass->getTypeName();
				typeInfo->mValueType = true;

				return typeInfo;
			}

			break;
		case MONO_TYPE_GENERICINST:
			if(monoClass->getFullName() == mSystemGenericListClass->getFullName()) // Full name is part of CIL spec, so it is just fine to compare like this
			{
				std::shared_ptr<ScriptSerializableTypeInfoList> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoList>();

				MonoProperty& itemProperty = monoClass->getProperty("Item");
				MonoClass* itemClass = itemProperty.getReturnType();

				if(itemClass != nullptr)
					typeInfo->mElementType = determineType(itemClass);

				return typeInfo;
			}
			else if(monoClass->getFullName() == mSystemGenericDictionaryClass->getFullName())
			{
				std::shared_ptr<ScriptSerializableTypeInfoDictionary> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoDictionary>();

				MonoMethod& getEnumerator = monoClass->getMethod("GetEnumerator");
				MonoClass* enumClass = getEnumerator.getReturnType();

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
				std::shared_ptr<ScriptSerializableTypeInfoArray> typeInfo = cm_shared_ptr<ScriptSerializableTypeInfoArray>();

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

	void RuntimeScriptObjects::clearScriptObjects(const String& assemblyName)
	{
		mAssemblyInfos.erase(assemblyName);

		mBaseTypesInitialized = false;

		mSystemArrayClass = nullptr;
		mSystemGenericListClass = nullptr;
		mSystemGenericDictionaryClass = nullptr;

		mSerializableObjectAttribute = nullptr;
		mDontSerializeFieldAttribute = nullptr;

		mComponentClass = nullptr;
		mSceneObjectClass = nullptr;

		mTextureClass = nullptr;
		mSpriteTextureClass = nullptr;

		mSerializeFieldAttribute = nullptr;
		mHideInInspectorAttribute = nullptr;
	}

	void RuntimeScriptObjects::initializeBaseTypes()
	{
		// Get necessary classes for detecting needed class & field information
		MonoAssembly* mscorlib = MonoManager::instance().getAssembly("mscorlib");
		if(mscorlib == nullptr)
			CM_EXCEPT(InvalidStateException, "mscorlib assembly is not loaded.");

		MonoAssembly* bansheeEngineAssembly = MonoManager::instance().getAssembly(BansheeEngineAssemblyName);
		if(bansheeEngineAssembly == nullptr)
			CM_EXCEPT(InvalidStateException, String(BansheeEngineAssemblyName) +  " assembly is not loaded.");

		mSystemArrayClass = mscorlib->getClass("System", "Array");
		if(mSystemArrayClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find System.Array managed class.");

		mSystemGenericListClass = mscorlib->getClass("System.Collections.Generic", "List`1");
		if(mSystemGenericListClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find List<T> managed class.");

		mSystemGenericDictionaryClass = mscorlib->getClass("System.Collections.Generic", "Dictionary`2");
		if(mSystemGenericDictionaryClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Dictionary<TKey, TValue> managed class.");

		mSerializableObjectAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "SerializableObject");
		if(mSerializableObjectAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SerializableObject managed class.");

		mDontSerializeFieldAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "DontSerializeField");
		if(mDontSerializeFieldAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find DontSerializeField managed class.");

		mComponentClass = bansheeEngineAssembly->getClass("BansheeEngine", "Component");
		if(mComponentClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Component managed class.");

		mSceneObjectClass = bansheeEngineAssembly->getClass("BansheeEngine", "SceneObject");
		if(mSceneObjectClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SceneObject managed class.");

		mTextureClass = bansheeEngineAssembly->getClass("BansheeEngine", "Texture2D");
		if(mTextureClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Texture2D managed class.");

		mSpriteTextureClass = bansheeEngineAssembly->getClass("BansheeEngine", "SpriteTexture");
		if(mSpriteTextureClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SpriteTexture managed class.");

		mSerializeFieldAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "SerializeField");
		if(mSerializeFieldAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SerializeField managed class.");

		mHideInInspectorAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "HideInInspector");
		if(mHideInInspectorAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find HideInInspector managed class.");

		mBaseTypesInitialized = true;
	}

	bool RuntimeScriptObjects::getSerializableObjectInfo(const String& ns, const String& typeName, std::shared_ptr<ScriptSerializableObjectInfo>& outInfo)
	{
		String fullName = ns + "." + typeName;
		for(auto& curAssembly : mAssemblyInfos)
		{
			auto iterFind = curAssembly.second->mTypeNameToId.find(fullName);
			if(iterFind != curAssembly.second->mTypeNameToId.end())
			{
				outInfo = curAssembly.second->mObjectInfos[iterFind->second];

				return true;
			}
		}

		return false;
	}

	bool RuntimeScriptObjects::hasSerializableObjectInfo(const String& ns, const String& typeName)
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