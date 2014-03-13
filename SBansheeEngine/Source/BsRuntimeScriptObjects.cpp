#include "BsRuntimeScriptObjects.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	SerializableObjectInfo::SerializableObjectInfo()
		:mMonoClass(nullptr), mTypeId(0)
	{

	}

	SerializableObjectInfo::~SerializableObjectInfo()
	{
		for(auto& field : mFields)
		{
			cm_delete(field.second);
		}
	}

	SerializableFieldInfo::SerializableFieldInfo()
		:mMonoField(nullptr), mType(ScriptFieldType::Other), mFlags((ScriptFieldFlags)0), mFieldId(0)
	{

	}

	SerializableFieldInfo::~SerializableFieldInfo()
	{

	}

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

		MonoClass* componentClass = bansheeEngineAssembly->getClass("BansheeEngine", "Component");
		if(componentClass == nullptr)
			CM_EXCEPT(InvalidStateException, "Cannot find Component managed class.");

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

		std::shared_ptr<SerializableAssemblyInfo> assemblyInfo = cm_shared_ptr<SerializableAssemblyInfo>();
		mAssemblyInfos[assemblyName] = assemblyInfo;

		const Vector<MonoClass*>::type& allClasses = curAssembly->getAllClasses();

		for(auto& curClass : allClasses)
		{
			if((curClass->isSubClassOf(componentClass) || curClass->hasAttribute(serializableAttribute)) && curClass != componentClass)
			{
				std::shared_ptr<SerializableObjectInfo> objInfo = cm_shared_ptr<SerializableObjectInfo>();

				objInfo->mTypeId = mUniqueTypeId++;
				objInfo->mTypeName = curClass->getTypeName();
				objInfo->mNamespace = curClass->getNamespace();
				objInfo->mMonoClass = curClass;

				::MonoClass* baseClass = mono_class_get_parent(curClass->_getInternalClass());
				if(baseClass != nullptr)
				{
					String dbgName = mono_class_get_name(baseClass);
				}

				String fullTypeName = objInfo->mNamespace + "." + objInfo->mTypeName;
				assemblyInfo->mTypeNameToId[fullTypeName] = objInfo->mTypeId;
				assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;

				CM::UINT32 mUniqueFieldId = 1;
				const CM::Vector<MonoField*>::type& fields = curClass->getAllFields();

				for(auto& field : fields)
				{
					if(field->isStatic())
						continue;

					SerializableFieldInfo* fieldInfo = cm_new<SerializableFieldInfo>();
					fieldInfo->mFieldId = mUniqueFieldId++;
					fieldInfo->mMonoField = field;
					fieldInfo->mName = field->getName();

					MonoClass* fieldType = field->getType();
					fieldInfo->mTypeNamespace = fieldType->getNamespace();
					fieldInfo->mTypeName = fieldType->getTypeName();
					
					MonoType* monoType = mono_class_get_type(fieldType->_getInternalClass());
					int monoPrimitiveType = mono_type_get_type(monoType);
					
					// TODO - We don't support nested arrays or multi-dimensional arrays
					bool isSupportedType = true;
					if(monoPrimitiveType == MONO_TYPE_ARRAY) 
					{
						::MonoClass* elementClass = mono_class_get_element_class(fieldType->_getInternalClass());
						if(elementClass != nullptr)
						{
							monoType = mono_class_get_type(elementClass);
							monoPrimitiveType = mono_type_get_type(monoType);
						}

						fieldInfo->mFlags = (ScriptFieldFlags)((UINT32)fieldInfo->mFlags | (UINT32)ScriptFieldFlags::Array);
					}
					// TODO - Also check for List and get its generic primitive

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
						// TODO - Check for Resource or GameObject ref, or SerializableObject
						break;
					case MONO_TYPE_VALUETYPE:
						// TODO - Check for SerializableObject
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
		}

		// Form parent/child connections
		for(auto& curClass : assemblyInfo->mObjectInfos)
		{
			MonoClass* base = curClass.second->mMonoClass->getBaseClass();
			while(base != nullptr)
			{
				std::shared_ptr<SerializableObjectInfo> baseObjInfo;
				if(getSerializableObjectInfo(base->getNamespace(), base->getTypeName(), baseObjInfo))
				{
					curClass.second->mBaseClass = baseObjInfo;
					baseObjInfo->mDerivedClasses.push_back(curClass.second);

					break;
				}

				base = base->getBaseClass();
			}
		}
	
		// TODO - How to handle arrays and/or Lists (or collections in general?)

		// TODO - In BsScriptEnginePlugin.cpp I create RuntimeScriptObjects class but never destroy it, for debug purposes. Remember to remove that.

		// TODO - Figure our field type (primitive, gameobject ref, etc.) - Should I do this here or while serializing? Probably here.
		//    - Add easy way to modify those fields (depending on their type) - Maybe make them virtual
		//  Detect field type:
		//   - Primitive - straightforward just check for primitive types
		//   - GameObjectHandle - check if object is SceneObject or derives from Component
		//   - SerializableObject - check if object is SerializableObject
		//   - ResourceHandle - derives from Resource

		// TOOD - Make SerializableObjectInfo IReflectable and create its RTTI
	}

	void RuntimeScriptObjects::clearScriptObjects(const CM::String& assemblyName)
	{
		mAssemblyInfos.erase(assemblyName);
	}

	bool RuntimeScriptObjects::getSerializableObjectInfo(const CM::String& ns, const CM::String& typeName, std::shared_ptr<SerializableObjectInfo>& outInfo)
	{
		String fullName = ns + "." + typeName;
		for(auto& curAssembly : mAssemblyInfos)
		{
			auto iterFind = curAssembly.second->mTypeNameToId.find(fullName);
			if(iterFind != curAssembly.second->mTypeNameToId.end())
			{
				outInfo = curAssembly.second->mObjectInfos[iterFind->first];

				return true;
			}
		}

		return false;
	}
}