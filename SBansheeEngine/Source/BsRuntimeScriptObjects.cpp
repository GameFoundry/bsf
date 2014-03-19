#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoUtil.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	RuntimeScriptObjects::~RuntimeScriptObjects()
	{

	}

	void RuntimeScriptObjects::refreshScriptObjects(const String& assemblyName)
	{
		clearScriptObjects(assemblyName);

		// Get necessary classes for detecting needed class & field information
		MonoAssembly* mscorlib = MonoManager::instance().getAssembly("mscorlib");
		if(mscorlib == nullptr)
			CM_EXCEPT(InvalidStateException, "mscorlib assembly is not loaded.");

		MonoAssembly* bansheeEngineAssembly = MonoManager::instance().getAssembly(BansheeEngineAssemblyName);
		if(bansheeEngineAssembly == nullptr)
			CM_EXCEPT(InvalidStateException, String(BansheeEngineAssemblyName) +  " assembly is not loaded.");

		MonoClass* serializableAttribute = mscorlib->getClass("System", "SerializableAttribute");
		if(serializableAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SerializableAttribute managed class.");

		MonoClass* nonSerializedAttribute = mscorlib->getClass("System", "NonSerializedAttribute");
		if(nonSerializedAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find NonSerializedAttribute managed class.");

		MonoClass* genericListClass = mscorlib->getClass("System", "List`1");
		if(genericListClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find List<T> managed class.");

		MonoClass* componentClass = bansheeEngineAssembly->getClass("BansheeEngine", "Component");
		if(componentClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Component managed class.");

		MonoClass* sceneObjectClass = bansheeEngineAssembly->getClass("BansheeEngine", "SceneObject");
		if(sceneObjectClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SceneObject managed class.");

		MonoClass* textureClass = bansheeEngineAssembly->getClass("BansheeEngine", "Texture2D");
		if(textureClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Texture2D managed class.");

		MonoClass* spriteTextureClass = bansheeEngineAssembly->getClass("BansheeEngine", "SpriteTexture");
		if(spriteTextureClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SpriteTexture managed class.");

		MonoClass* serializeFieldAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "SerializeField");
		if(serializeFieldAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find SerializeField managed class.");

		MonoClass* hideInInspectorAttribute = bansheeEngineAssembly->getClass("BansheeEngine", "HideInInspector");
		if(hideInInspectorAttribute == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find HideInInspector managed class.");
		
		// Process all classes and fields
		CM::UINT32 mUniqueTypeId = 1;

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
			if((curClass->isSubClassOf(componentClass) || curClass->hasAttribute(serializableAttribute)) && curClass != componentClass)
			{
				std::shared_ptr<ScriptSerializableObjectInfo> objInfo = cm_shared_ptr<ScriptSerializableObjectInfo>();

				objInfo->mTypeId = mUniqueTypeId++;
				objInfo->mTypeName = curClass->getTypeName();
				objInfo->mNamespace = curClass->getNamespace();
				objInfo->mMonoClass = curClass;

				assemblyInfo->mTypeNameToId[objInfo->getFullTypeName()] = objInfo->mTypeId;
				assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;
			}
		}

		// Populate field data
		for(auto& curClassInfo : assemblyInfo->mObjectInfos)
		{
			std::shared_ptr<ScriptSerializableObjectInfo> objInfo = curClassInfo.second;

			String fullTypeName = objInfo->mNamespace + "." + objInfo->mTypeName;
			assemblyInfo->mTypeNameToId[fullTypeName] = objInfo->mTypeId;
			assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;

			CM::UINT32 mUniqueFieldId = 1;
			const CM::Vector<MonoField*>::type& fields = objInfo->mMonoClass->getAllFields();

			for(auto& field : fields)
			{
				if(field->isStatic())
					continue;

				MonoClass* fieldType = field->getType();
				MonoClass* fieldElementClass = fieldType;
				MonoType* monoType = mono_class_get_type(fieldType->_getInternalClass());
				int monoPrimitiveType = mono_type_get_type(monoType);

				std::shared_ptr<ScriptSerializableFieldInfo> fieldInfo = nullptr;

				bool isSupportedType = true;
				if(monoPrimitiveType == MONO_TYPE_ARRAY) 
				{
					std::shared_ptr<ScriptSerializableFieldInfoArray> arrayFieldInfo = cm_shared_ptr<ScriptSerializableFieldInfoArray>();
					arrayFieldInfo->mArrayDimensions = (UINT32)mono_class_get_rank(fieldType->_getInternalClass());

					fieldInfo = arrayFieldInfo;

					::MonoClass* elementClass = mono_class_get_element_class(fieldType->_getInternalClass());
					if(elementClass != nullptr)
					{
						monoType = mono_class_get_type(elementClass);
						monoPrimitiveType = mono_type_get_type(monoType);

						::MonoClass* elementClass = mono_type_get_class(monoType);
						String elementNs = mono_class_get_namespace(elementClass);
						String elementTypeName = mono_class_get_name(elementClass);

						fieldElementClass = MonoManager::instance().findClass(elementNs, elementTypeName);
					}
				}
				else if(monoPrimitiveType == MONO_TYPE_CLASS)
				{
					// TODO - Check for List or Dictionary
					
					// If not List/Dictionary
					fieldInfo = cm_shared_ptr<ScriptSerializableFieldInfoPlain>();
				}
				else
				{
					fieldInfo = cm_shared_ptr<ScriptSerializableFieldInfoPlain>();
				}

				fieldInfo->mFieldId = mUniqueFieldId++;
				fieldInfo->mMonoField = field;
				fieldInfo->mName = field->getName();

				fieldInfo->mTypeNamespace = fieldType->getNamespace();
				fieldInfo->mTypeName = fieldType->getTypeName();

				//  Determine field type
				switch(monoPrimitiveType) // TODO - If array I need to get underlying type
				{
				case MONO_TYPE_BOOLEAN:
					fieldInfo->mType = ScriptFieldType::Bool;
					break;
				case MONO_TYPE_CHAR:
					fieldInfo->mType = ScriptFieldType::Char;
					break;
				case MONO_TYPE_I1:
					fieldInfo->mType = ScriptFieldType::I8;
					break;
				case MONO_TYPE_U1:
					fieldInfo->mType = ScriptFieldType::U8;
					break;
				case MONO_TYPE_I2:
					fieldInfo->mType = ScriptFieldType::I16;
					break;
				case MONO_TYPE_U2:
					fieldInfo->mType = ScriptFieldType::U16;
					break;
				case MONO_TYPE_I4:
					fieldInfo->mType = ScriptFieldType::I32;
					break;
				case MONO_TYPE_U4:
					fieldInfo->mType = ScriptFieldType::U32;
					break;
				case MONO_TYPE_I8:
					fieldInfo->mType = ScriptFieldType::U64;
					break;
				case MONO_TYPE_U8:
					fieldInfo->mType = ScriptFieldType::U64;
					break;
				case MONO_TYPE_STRING:
					fieldInfo->mType = ScriptFieldType::String;
					break;
				case MONO_TYPE_R4:
					fieldInfo->mType = ScriptFieldType::Float;
					break;
				case MONO_TYPE_R8:
					fieldInfo->mType = ScriptFieldType::Double;
					break;
				case MONO_TYPE_CLASS:
					if(fieldElementClass->isSubClassOf(textureClass))
						fieldInfo->mType = ScriptFieldType::TextureRef;
					else if(fieldElementClass->isSubClassOf(spriteTextureClass))
						fieldInfo->mType = ScriptFieldType::SpriteTextureRef;
					else if(fieldElementClass->isSubClassOf(sceneObjectClass))
						fieldInfo->mType = ScriptFieldType::SceneObjectRef;
					else if(fieldElementClass->isSubClassOf(componentClass))
						fieldInfo->mType = ScriptFieldType::ComponentRef;
					else
					{
						if(hasSerializableObjectInfo(fieldElementClass->getNamespace(), fieldElementClass->getTypeName())) // TODO - Ensure this checks for array types
							fieldInfo->mType = ScriptFieldType::SerializableObjectRef;
					}

					break;
				case MONO_TYPE_VALUETYPE:
					if(hasSerializableObjectInfo(fieldElementClass->getNamespace(), fieldElementClass->getTypeName()))
						fieldInfo->mType = ScriptFieldType::SerializableObjectValue;

					break;
				}

				if(fieldInfo->mType != ScriptFieldType::Other)
				{
					MonoFieldVisibility visibility = field->getVisibility();
					if(visibility == MonoFieldVisibility::Public)
					{
						if(!field->hasAttribute(nonSerializedAttribute))
							fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Serializable);

						if(!field->hasAttribute(hideInInspectorAttribute))
							fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Inspectable);
					}
					else
					{
						if(field->hasAttribute(serializeFieldAttribute))
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

	void RuntimeScriptObjects::clearScriptObjects(const CM::String& assemblyName)
	{
		mAssemblyInfos.erase(assemblyName);
	}

	bool RuntimeScriptObjects::getSerializableObjectInfo(const CM::String& ns, const CM::String& typeName, std::shared_ptr<ScriptSerializableObjectInfo>& outInfo)
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

	bool RuntimeScriptObjects::hasSerializableObjectInfo(const CM::String& ns, const CM::String& typeName)
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