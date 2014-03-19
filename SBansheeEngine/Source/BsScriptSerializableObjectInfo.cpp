#include "BsScriptSerializableObjectInfo.h"
#include "BsScriptSerializableObjectInfoRTTI.h"
#include "BsMonoUtil.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptComponent.h"
#include "BsScriptSceneObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	RTTITypeBase* ScriptSerializableAssemblyInfo::getRTTIStatic()
	{
		return ScriptSerializableAssemblyInfoRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableAssemblyInfo::getRTTI() const
	{
		return ScriptSerializableAssemblyInfo::getRTTIStatic();
	}

	ScriptSerializableObjectInfo::ScriptSerializableObjectInfo()
		:mMonoClass(nullptr), mTypeId(0)
	{

	}

	RTTITypeBase* ScriptSerializableObjectInfo::getRTTIStatic()
	{
		return ScriptSerializableObjectInfoRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableObjectInfo::getRTTI() const
	{
		return ScriptSerializableObjectInfo::getRTTIStatic();
	}

	ScriptSerializableFieldInfo::ScriptSerializableFieldInfo()
		:mMonoField(nullptr), mType(ScriptFieldType::Other), mFlags((ScriptFieldFlags)0), mFieldId(0), mArrayDimensions(0)
	{

	}

	bool ScriptSerializableFieldInfo::isNull(MonoObject* obj)
	{
		assert(isReferenceType());

		void* val = mMonoField->getValue(obj);

		return val == nullptr;
	}

	void ScriptSerializableFieldInfo::setNull(MonoObject* obj)
	{
		assert(isReferenceType());

		mMonoField->setValue(obj, nullptr);
	}

	void ScriptSerializableFieldInfo::setU8(MonoObject* obj, UINT8 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U8);

		setValue(obj, &val, arrayIdx);
	}

	UINT8 ScriptSerializableFieldInfo::getU8(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U8);

		return *(UINT8*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setI8(MonoObject* obj, INT8 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I8);

		setValue(obj, &val, arrayIdx);
	}

	INT8 ScriptSerializableFieldInfo::getI8(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I8);

		return *(INT8*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setU16(MonoObject* obj, UINT16 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U16);

		setValue(obj, &val, arrayIdx);
	}

	UINT16 ScriptSerializableFieldInfo::getU16(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U16);

		return *(UINT16*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setI16(MonoObject* obj, INT16 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I16);

		setValue(obj, &val, arrayIdx);
	}

	INT16 ScriptSerializableFieldInfo::getI16(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I16);

		return *(INT16*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setU32(MonoObject* obj, UINT32 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U32);

		setValue(obj, &val, arrayIdx);
	}

	UINT32 ScriptSerializableFieldInfo::getU32(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U32);

		return *(UINT32*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setI32(MonoObject* obj, INT32 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I32);

		setValue(obj, &val, arrayIdx);
	}

	INT32 ScriptSerializableFieldInfo::getI32(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I32);

		return *(INT32*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setU64(MonoObject* obj, UINT64 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U64);

		setValue(obj, &val, arrayIdx);
	}

	UINT64 ScriptSerializableFieldInfo::getU64(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::U64);

		return *(UINT64*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setI64(MonoObject* obj, INT64 val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I64);

		setValue(obj, &val, arrayIdx);
	}

	INT64 ScriptSerializableFieldInfo::getI64(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::I64);

		return *(INT64*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setBool(MonoObject* obj, bool val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Bool);

		setValue(obj, &val, arrayIdx);
	}

	bool ScriptSerializableFieldInfo::getBool(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Bool);

		return *(bool*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setChar(MonoObject* obj, wchar_t val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Char);

		setValue(obj, &val, arrayIdx);
	}

	wchar_t ScriptSerializableFieldInfo::getChar(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Char);

		return *(wchar_t*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setFloat(MonoObject* obj, float val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Float);

		setValue(obj, &val, arrayIdx);
	}

	float ScriptSerializableFieldInfo::getFloat(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Float);

		return *(float*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setDouble(MonoObject* obj, double val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Double);

		setValue(obj, &val, arrayIdx);
	}

	double ScriptSerializableFieldInfo::getDouble(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::Double);

		return *(double*)getValue(obj, arrayIdx);
	}

	void ScriptSerializableFieldInfo::setString(MonoObject* obj, const WString& val, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::String);

		MonoString* str = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), val);
		setValue(obj, str, arrayIdx);
	}

	WString ScriptSerializableFieldInfo::getString(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::String);

		MonoString* str = (MonoString*)getValue(obj, arrayIdx);
		if(str == nullptr)
			return L"";

		return MonoUtil::monoToWString(str);
	}

	void ScriptSerializableFieldInfo::setTexture(MonoObject* obj, const HTexture& resource, UINT32 arrayIdx)
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

	CM::HTexture ScriptSerializableFieldInfo::getTexture(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::TextureRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HTexture();

		ScriptTexture2D* scriptResource = ScriptTexture2D::toNative(managedInstance);
		return static_resource_cast<Texture>(scriptResource->getNativeHandle());
	}

	void ScriptSerializableFieldInfo::setSpriteTexture(MonoObject* obj, const HSpriteTexture& resource, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SpriteTextureRef);

		if(resource == nullptr)
		{
			setValue(obj, nullptr, arrayIdx);
		}
		else
		{
			ScriptSpriteTexture* scriptResource = ScriptResourceManager::instance().getScriptSpriteTexture(resource);
			if(scriptResource == nullptr)
				scriptResource = ScriptResourceManager::instance().createScriptSpriteTexture(resource);

			MonoObject* managedInstance = scriptResource->getManagedInstance();
			setValue(obj, (void*)managedInstance, arrayIdx);
		}
	}

	HSpriteTexture ScriptSerializableFieldInfo::getSpriteTexture(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SpriteTextureRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HTexture();

		ScriptSpriteTexture* scriptResource = ScriptSpriteTexture::toNative(managedInstance);
		return static_resource_cast<SpriteTexture>(scriptResource->getNativeHandle());
	}

	void ScriptSerializableFieldInfo::setSceneObject(MonoObject* obj, const HSceneObject& sceneObject, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SceneObjectRef);

		if(sceneObject == nullptr)
		{
			setValue(obj, nullptr, arrayIdx);
		}
		else
		{
			ScriptSceneObject* scriptSceneObject = ScriptGameObjectManager::instance().getScriptSceneObject(sceneObject);
			if(scriptSceneObject == nullptr)
				scriptSceneObject = ScriptGameObjectManager::instance().createScriptSceneObject(sceneObject);

			MonoObject* managedInstance = scriptSceneObject->getManagedInstance();
			setValue(obj, (void*)managedInstance, arrayIdx);
		}
	}

	HSceneObject ScriptSerializableFieldInfo::getSceneObject(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SceneObjectRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HSceneObject();

		ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(managedInstance);
		return static_object_cast<SceneObject>(scriptSceneObject->getNativeHandle());
	}

	void ScriptSerializableFieldInfo::setComponent(MonoObject* obj, const HComponent& component, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::ComponentRef);

		if(component == nullptr)
		{
			setValue(obj, nullptr, arrayIdx);
		}
		else
		{
			ScriptComponent* scriptComponent = ScriptGameObjectManager::instance().getScriptComponent(component);
			if(scriptComponent == nullptr)
				scriptComponent = ScriptGameObjectManager::instance().createScriptComponent(component);

			MonoObject* managedInstance = scriptComponent->getManagedInstance();
			setValue(obj, (void*)managedInstance, arrayIdx);
		}
	}

	HComponent ScriptSerializableFieldInfo::getComponent(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::ComponentRef);

		MonoObject* managedInstance = (MonoObject*)getValue(obj, arrayIdx);
		if(managedInstance == nullptr)
			return HComponent();

		ScriptComponent* scriptComponent = ScriptComponent::toNative(managedInstance);
		return static_object_cast<Component>(scriptComponent->getNativeHandle());
	}


	void ScriptSerializableFieldInfo::setSerializableObject(MonoObject* obj, const MonoObject* value, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SerializableObjectRef || mType == ScriptFieldType::SerializableObjectValue);

		setValue(obj, (void*)value, arrayIdx);
	}

	MonoObject* ScriptSerializableFieldInfo::getSerializableObject(MonoObject* obj, UINT32 arrayIdx)
	{
		assert(mType == ScriptFieldType::SerializableObjectRef || mType == ScriptFieldType::SerializableObjectValue);

		return (MonoObject*)getValue(obj, arrayIdx);
	}

	RTTITypeBase* ScriptSerializableFieldInfo::getRTTIStatic()
	{
		return ScriptSerializableFieldInfoRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldInfo::getRTTI() const
	{
		return ScriptSerializableFieldInfo::getRTTIStatic();
	}

	bool ScriptSerializableFieldInfoPlain::isReferenceType() const
	{
		return (mType == ScriptFieldType::TextureRef || mType == ScriptFieldType::SpriteTextureRef || 
			mType == ScriptFieldType::SceneObjectRef || mType == ScriptFieldType::ComponentRef || mType == ScriptFieldType::SerializableObjectRef);
	}

	CM::UINT32 ScriptSerializableFieldInfoPlain::getNumArrayElements(MonoObject* obj, CM::UINT32 dimension)
	{
		assert(false);

		return 0;
	}

	void ScriptSerializableFieldInfoPlain::setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, CM::UINT32 dimension, bool discardExisting)
	{
		assert(false);
	}

	void ScriptSerializableFieldInfoPlain::setValue(MonoObject* obj, void* val, CM::UINT32 arrayIdx)
	{
		assert(arrayIdx == 0);

		mMonoField->setValue(obj, val);
	}

	void* ScriptSerializableFieldInfoPlain::getValue(MonoObject* obj, CM::UINT32 arrayIdx)
	{
		assert(arrayIdx == 0);

		return mMonoField->getValue(obj);
	}

	RTTITypeBase* ScriptSerializableFieldInfoPlain::getRTTIStatic()
	{
		return ScriptSerializableFieldInfoPlainRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldInfoPlain::getRTTI() const
	{
		return ScriptSerializableFieldInfoPlain::getRTTIStatic();
	}

	ScriptSerializableFieldInfoArray::ScriptSerializableFieldInfoArray()
		:mArrayDimensions(0)
	{

	}

	CM::UINT32 ScriptSerializableFieldInfoArray::getNumArrayElements(MonoObject* obj, CM::UINT32 dimension)
	{
		if(dimension >= mArrayDimensions)
			CM_EXCEPT(InvalidParametersException, "Invalid array dimension: " + toString(dimension) + ". Valid range is: [0, + " + toString(mArrayDimensions - 1) + "]");

		MonoArray* array = reinterpret_cast<MonoArray*>(mMonoField->getValue(obj));
		return (UINT32)mono_array_length(array);
	}

	void ScriptSerializableFieldInfoArray::setNumArrayElements(MonoObject* obj, CM::UINT32 numElements, CM::UINT32 dimension, bool discardExisting)
	{
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

	void ScriptSerializableFieldInfoArray::setValue( MonoObject* obj, void* val, CM::UINT32 arrayIdx)
	{
		MonoArray* array = (MonoArray*)mMonoField->getValue(obj);
		UINT32 elemSize = mono_array_element_size(mMonoField->getType()->_getInternalClass());

		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);

		void* elemAddr = mono_array_addr_with_size(array, elemSize, arrayIdx);
		memcpy(elemAddr, val, elemSize);
	}

	void* ScriptSerializableFieldInfoArray::getValue( MonoObject* obj, CM::UINT32 arrayIdx)
	{
		MonoArray* array = (MonoArray*)mMonoField->getValue(obj);
		UINT32 elemSize = mono_array_element_size(mMonoField->getType()->_getInternalClass());

		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);

		return mono_array_addr_with_size(array, elemSize, arrayIdx);
	}

	RTTITypeBase* ScriptSerializableFieldInfoArray::getRTTIStatic()
	{
		return ScriptSerializableFieldInfoArrayRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldInfoArray::getRTTI() const
	{
		return ScriptSerializableFieldInfoArray::getRTTIStatic();
	}
}