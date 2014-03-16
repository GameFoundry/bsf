#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoUtil.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"

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

	bool SerializableFieldInfo::isArray()
	{
		return ((UINT32)mFlags & (UINT32)ScriptFieldFlags::Array) != 0;
	}

	bool SerializableFieldInfo::isReferenceType()
	{
		return (isArray() || mType == ScriptFieldType::TextureRef || mType == ScriptFieldType::SpriteTextureRef || 
			mType == ScriptFieldType::SceneObjectRef || mType == ScriptFieldType::ComponentRef || mType == ScriptFieldType::SerializableObjectRef);
	}

	bool SerializableFieldInfo::isNull(MonoObject* obj)
	{
		assert(isReferenceType());

		void* val = mMonoField->getValue(obj);

		return val == nullptr;
	}

	void SerializableFieldInfo::setNull(MonoObject* obj)
	{
		assert(isReferenceType());

		mMonoField->setValue(obj, nullptr);
	}

	UINT32 SerializableFieldInfo::getNumArrayElements(MonoObject* obj)
	{
		assert(((UINT32)mFlags & (UINT32)ScriptFieldFlags::Array) != 0);

		MonoArray* array = reinterpret_cast<MonoArray*>(mMonoField->getValue(obj));
		return (UINT32)mono_array_length(array);
	}

	void SerializableFieldInfo::setNumArrayElements(MonoObject* obj, UINT32 numElements, bool discardExisting)
	{
		assert(((UINT32)mFlags & (UINT32)ScriptFieldFlags::Array) != 0);

		uint32_t lengths[1] = { numElements };

		MonoArray* newArray = mono_array_new_full(MonoManager::instance().getDomain(), 
			mMonoField->getType()->_getInternalClass(), (uintptr_t*)lengths, nullptr);

		if(!discardExisting)
		{
			MonoArray* existingArray = reinterpret_cast<MonoArray*>(mMonoField->getValue(obj));
			UINT32 existingArrayLength = (UINT32)mono_array_length(existingArray);

			UINT32 elemsToCopy = std::min(existingArrayLength, numElements);
			int32_t elemSize = mono_array_element_size(mMonoField->getType()->_getInternalClass());

			for(UINT32 i = 0; i < elemsToCopy; i++)
			{
				void* existingValAddr = (void*)mono_array_addr_with_size(existingArray, elemSize, (uintptr_t)i);
				void* newValAddr = (void*)mono_array_addr_with_size(newArray, elemSize, (uintptr_t)i);

				memcpy(newValAddr, existingValAddr, elemSize);
			}
		}

		mMonoField->setValue(obj, newArray);
	}

	void SerializableFieldInfo::setU8(MonoObject* obj, UINT8 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U8);

		setValue(obj, &val, arrayIdx);
	}

	UINT8 SerializableFieldInfo::getU8(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U8);

		return *(UINT8*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setI8(MonoObject* obj, INT8 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I8);

		setValue(obj, &val, arrayIdx);
	}

	INT8 SerializableFieldInfo::getI8(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I8);

		return *(INT8*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setU16(MonoObject* obj, UINT16 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U16);

		setValue(obj, &val, arrayIdx);
	}

	UINT16 SerializableFieldInfo::getU16(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U16);

		return *(UINT16*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setI16(MonoObject* obj, INT16 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I16);

		setValue(obj, &val, arrayIdx);
	}

	INT16 SerializableFieldInfo::getI16(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I16);

		return *(INT16*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setU32(MonoObject* obj, UINT32 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U32);

		setValue(obj, &val, arrayIdx);
	}

	UINT32 SerializableFieldInfo::getU32(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U32);

		return *(UINT32*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setI32(MonoObject* obj, INT32 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I32);

		setValue(obj, &val, arrayIdx);
	}

	INT32 SerializableFieldInfo::getI32(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I32);

		return *(INT32*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setU64(MonoObject* obj, UINT64 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U64);

		setValue(obj, &val, arrayIdx);
	}

	UINT64 SerializableFieldInfo::getU64(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U64);

		return *(UINT64*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setI64(MonoObject* obj, INT64 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I64);

		setValue(obj, &val, arrayIdx);
	}

	INT64 SerializableFieldInfo::getI64(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I64);

		return *(INT64*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setBool(MonoObject* obj, bool val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Bool);

		setValue(obj, &val, arrayIdx);
	}

	bool SerializableFieldInfo::getBool(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Bool);

		return *(bool*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setChar(MonoObject* obj, wchar_t val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Char);

		setValue(obj, &val, arrayIdx);
	}

	wchar_t SerializableFieldInfo::getChar(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Char);

		return *(wchar_t*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setFloat(MonoObject* obj, float val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Float);

		setValue(obj, &val, arrayIdx);
	}

	float SerializableFieldInfo::getFloat(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Float);

		return *(float*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setDouble(MonoObject* obj, double val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Double);

		setValue(obj, &val, arrayIdx);
	}

	double SerializableFieldInfo::getDouble(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Double);

		return *(double*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setString(MonoObject* obj, const WString& val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::String);

		MonoString* str = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), val);
		setValue(obj, str, arrayIdx);
	}

	WString SerializableFieldInfo::getString(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::String);

		MonoString* str = (MonoString*)getValue(obj, arrayIdx);
		if(str == nullptr)
			return L"";

		return MonoUtil::monoToWString(str);
	}

	void SerializableFieldInfo::setTexture(MonoObject* obj, const HTexture& resource, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::TextureRef);

		if(resource == nullptr)
		{
			setValue(obj, nullptr, arrayIdx);
		}
		else
		{
			ScriptTexture2D* scriptResource = ScriptResourceManager::instance().getScriptTexture(resource);
			if(scriptResource == nullptr)
				scriptResource = ScriptResourceManager::instance().createScriptTexture(resource);

			MonoObject* managedInstance = scriptResource->getManagedInstance();
			setValue(obj, (void*)managedInstance, arrayIdx);
		}
	}

	CM::HTexture SerializableFieldInfo::getTexture(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::TextureRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HTexture();

		ScriptTexture2D* scriptResource = ScriptTexture2D::toNative(managedInstance);
		return static_resource_cast<Texture>(scriptResource->getNativeHandle());
	}

	void SerializableFieldInfo::setSpriteTexture(MonoObject* obj, const HSpriteTexture& resource, UINT32 arrayIdx)
	{
		if(resource == nullptr)
		{
			setValue(obj, nullptr, arrayIdx);
		}
		else
		{
			assert(mType == ScriptFieldType::SpriteTextureRef);

			ScriptSpriteTexture* scriptResource = ScriptResourceManager::instance().getScriptSpriteTexture(resource);
			if(scriptResource == nullptr)
				scriptResource = ScriptResourceManager::instance().createScriptSpriteTexture(resource);

			MonoObject* managedInstance = scriptResource->getManagedInstance();
			setValue(obj, (void*)managedInstance, arrayIdx);
		}
	}

	HSpriteTexture SerializableFieldInfo::getSpriteTexture(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SpriteTextureRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HTexture();

		ScriptSpriteTexture* scriptResource = ScriptSpriteTexture::toNative(managedInstance);
		return static_resource_cast<SpriteTexture>(scriptResource->getNativeHandle());
	}

	void SerializableFieldInfo::setSceneObject(MonoObject* obj, const HSceneObject& sceneObject, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SceneObjectRef);

		// TODO
	}

	HSceneObject SerializableFieldInfo::getSceneObject(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SceneObjectRef);

		// TODO
		return HSceneObject();
	}

	void SerializableFieldInfo::setComponent(MonoObject* obj, const HComponent& component, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::ComponentRef);

		// TODO
	}

	HComponent SerializableFieldInfo::getComponent(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::ComponentRef);

		// TODO
		return HComponent();
	}


	void SerializableFieldInfo::setSerializableObject(MonoObject* obj, const MonoObject* value, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SerializableObjectRef || mType == ScriptFieldType::SerializableObjectValue);

		setValue(obj, (void*)value, arrayIdx);
	}

	MonoObject* SerializableFieldInfo::getSerializableObject(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SerializableObjectRef || mType == ScriptFieldType::SerializableObjectValue);

		return (MonoObject*)getValue(obj, arrayIdx);
	}

	void SerializableFieldInfo::setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx)
	{
		if(isArray())
		{
			MonoArray* array = (MonoArray*)mMonoField->getValue(obj);
			UINT32 elemSize = mono_array_element_size(mMonoField->getType()->_getInternalClass());

			UINT32 numElems = (UINT32)mono_array_length(array);
			assert(arrayIdx < numElems);

			void* elemAddr = mono_array_addr_with_size(array, elemSize, arrayIdx);
			memcpy(elemAddr, val, elemSize);
		}
		else
		{
			assert(arrayIdx == 0);

			mMonoField->setValue(obj, val);
		}
	}

	void* SerializableFieldInfo::getValue(MonoObject* obj, CM::UINT32 arrayIdx)
	{
		if(isArray())
		{
			MonoArray* array = (MonoArray*)mMonoField->getValue(obj);
			UINT32 elemSize = mono_array_element_size(mMonoField->getType()->_getInternalClass());

			UINT32 numElems = (UINT32)mono_array_length(array);
			assert(arrayIdx < numElems);

			return mono_array_addr_with_size(array, elemSize, arrayIdx);
		}
		else
		{
			assert(arrayIdx == 0);

			return mMonoField->getValue(obj);
		}
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

		std::shared_ptr<SerializableAssemblyInfo> assemblyInfo = cm_shared_ptr<SerializableAssemblyInfo>();
		mAssemblyInfos[assemblyName] = assemblyInfo;

		// Populate class data
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

				String fullTypeName = objInfo->mNamespace + "." + objInfo->mTypeName;
				assemblyInfo->mTypeNameToId[fullTypeName] = objInfo->mTypeId;
				assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;
			}
		}

		// Populate field data
		for(auto& curClassInfo : assemblyInfo->mObjectInfos)
		{
			std::shared_ptr<SerializableObjectInfo> objInfo = curClassInfo.second;

			String fullTypeName = objInfo->mNamespace + "." + objInfo->mTypeName;
			assemblyInfo->mTypeNameToId[fullTypeName] = objInfo->mTypeId;
			assemblyInfo->mObjectInfos[objInfo->mTypeId] = objInfo;

			CM::UINT32 mUniqueFieldId = 1;
			const CM::Vector<MonoField*>::type& fields = objInfo->mMonoClass->getAllFields();

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

				MonoClass* fieldElementClass = fieldType;
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

						::MonoClass* elementClass = mono_type_get_class(monoType);
						String elementNs = mono_class_get_namespace(elementClass);
						String elementTypeName = mono_class_get_name(elementClass);

						fieldElementClass = MonoManager::instance().findClass(elementNs, elementTypeName);
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
						if(hasSerializableObjectInfo(fieldElementClass->getNamespace(), fieldElementClass->getTypeName()))
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