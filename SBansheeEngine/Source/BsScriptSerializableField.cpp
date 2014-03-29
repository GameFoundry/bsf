#include "BsScriptSerializableField.h"
#include "BsScriptSerializableObjectInfo.h"
#include "BsScriptSerializableFieldRTTI.h"
#include "BsMonoUtil.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptSceneObject.h"
#include "BsScriptComponent.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableArray.h"
#include "BsScriptSerializableList.h"
#include "BsScriptSerializableDictionary.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableFieldKeyPtr ScriptSerializableFieldKey::create(CM::UINT16 typeId, CM::UINT16 fieldId)
	{
		ScriptSerializableFieldKeyPtr fieldKey = cm_shared_ptr<ScriptSerializableFieldKey>();
		fieldKey->mTypeId = typeId;
		fieldKey->mFieldId = fieldId;

		return fieldKey;
	}

	ScriptSerializableFieldDataEntryPtr ScriptSerializableFieldDataEntry::create(const ScriptSerializableFieldKeyPtr& key, const ScriptSerializableFieldDataPtr& value)
	{
		ScriptSerializableFieldDataEntryPtr fieldDataEntry = cm_shared_ptr<ScriptSerializableFieldDataEntry>();
		fieldDataEntry->mKey = key;
		fieldDataEntry->mValue = value;

		return fieldDataEntry;
	}

	ScriptSerializableFieldDataPtr ScriptSerializableFieldData::create(const ScriptSerializableTypeInfoPtr& typeInfo, MonoObject* value)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			switch (primitiveTypeInfo->mType)
			{
			case ScriptPrimitiveType::Bool:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataBool>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Char:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataChar>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I8:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U8:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I16:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U16:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I32:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U32:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I64:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U64:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Float:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataFloat>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Double:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataDouble>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::String:
				{
					MonoString* strVal = (MonoString*)(value);

					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataString>();
					if(strVal != nullptr)
						fieldData->value = MonoUtil::monoToWString(strVal);
					return fieldData;
				}
			case ScriptPrimitiveType::TextureRef:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();

					if(value != nullptr)
					{
						ScriptTexture2D* scriptTexture2D = ScriptTexture2D::toNative(value);
						fieldData->value = static_resource_cast<ScriptTexture2D>(scriptTexture2D->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SpriteTextureRef:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();
					
					if(value != nullptr)
					{
						ScriptSpriteTexture* scriptSpriteTexture = ScriptSpriteTexture::toNative(value);
						fieldData->value = static_resource_cast<SpriteTexture>(scriptSpriteTexture->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SceneObjectRef:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();

					if(value != nullptr)
					{
						ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(value);
						fieldData->value = static_object_cast<SceneObject>(scriptSceneObject->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::ComponentRef:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();

					if(value != nullptr)
					{
						ScriptComponent* scriptComponent = ScriptComponent::toNative(value);
						fieldData->value = static_object_cast<Component>(scriptComponent->getNativeHandle());
					}

					return fieldData;
				}
			}
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataObject>();
			if(value != nullptr)
			{
				fieldData->value = ScriptSerializableObject::create(value);
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataArray>();
			if(value != nullptr)
			{
				fieldData->value = ScriptSerializableArray::create(value, std::static_pointer_cast<ScriptSerializableTypeInfoArray>(typeInfo));
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataList>();
			if(value != nullptr)
			{
				fieldData->value = ScriptSerializableList::create(value, std::static_pointer_cast<ScriptSerializableTypeInfoList>(typeInfo));
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataDictionary>();
			if(value != nullptr)
			{
				fieldData->value = ScriptSerializableDictionary::create(value, std::static_pointer_cast<ScriptSerializableTypeInfoDictionary>(typeInfo));
			}

			return fieldData;
		}

		return nullptr;
	}

	void* ScriptSerializableFieldDataBool::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Bool)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataChar::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Char)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataI8::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I8)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataU8::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U8)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataI16::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I16)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataU16::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U16)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataI32::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I32)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataU32::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U32)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataI64::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I64)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataU64::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U64)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataFloat::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Float)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataDouble::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Double)
				return &value;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataString::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::String)
			{
				return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), value);
			}
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataResourceRef::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);

			if(primitiveTypeInfo->mType == ScriptPrimitiveType::TextureRef)
			{
				if(value)
				{
					ScriptTexture2D* scriptResource = ScriptResourceManager::instance().getScriptTexture(value);
					if(scriptResource == nullptr)
						scriptResource = ScriptResourceManager::instance().createScriptTexture(value);

					return scriptResource->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::SpriteTextureRef)
			{
				if(value)
				{
					ScriptSpriteTexture* scriptResource = ScriptResourceManager::instance().getScriptSpriteTexture(value);
					if(scriptResource == nullptr)
						scriptResource = ScriptResourceManager::instance().createScriptSpriteTexture(value);

					if(scriptResource != nullptr)
						return scriptResource->getManagedInstance();
				}
				else
					return nullptr;
			}
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataGameObjectRef::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);

			if(primitiveTypeInfo->mType == ScriptPrimitiveType::SceneObjectRef)
			{
				if(value)
				{
					ScriptSceneObject* scriptSceneObject = ScriptGameObjectManager::instance().getScriptSceneObject(value);
					if(scriptSceneObject == nullptr)
						scriptSceneObject = ScriptGameObjectManager::instance().createScriptSceneObject(value);

					return scriptSceneObject->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::ComponentRef)
			{
				if(value)
				{
					ScriptComponent* scriptComponent = ScriptGameObjectManager::instance().getScriptComponent(value);
					if(scriptComponent == nullptr)
						scriptComponent = ScriptGameObjectManager::instance().createScriptComponent(value);

					return scriptComponent->getManagedInstance();
				}
				else
					return nullptr;
			}
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataObject::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto objectTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoObject>(typeInfo);

			if(value != nullptr)
			{
				if(objectTypeInfo->mValueType)
				{
					MonoObject* managedInstance = value->getManagedInstance();
					
					if(managedInstance != nullptr)
						return mono_object_unbox(managedInstance); // Structs are passed as raw types because mono expects them as such
				}
				else
					return value->getManagedInstance();
			}

			return nullptr;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataArray::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto objectTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoArray>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataList::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			auto listTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoList>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataDictionary::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			auto dictionaryTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoDictionary>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	RTTITypeBase* ScriptSerializableFieldKey::getRTTIStatic()
	{
		return ScriptSerializableFieldKeyRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldKey::getRTTI() const
	{
		return ScriptSerializableFieldKey::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldData::getRTTIStatic()
	{
		return ScriptSerializableFieldDataRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldData::getRTTI() const
	{
		return ScriptSerializableFieldData::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataEntry::getRTTIStatic()
	{
		return ScriptSerializableFieldDataEntryRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataEntry::getRTTI() const
	{
		return ScriptSerializableFieldDataEntry::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataBool::getRTTIStatic()
	{
		return ScriptSerializableFieldDataBoolRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataBool::getRTTI() const
	{
		return ScriptSerializableFieldDataBool::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataChar::getRTTIStatic()
	{
		return ScriptSerializableFieldDataCharRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataChar::getRTTI() const
	{
		return ScriptSerializableFieldDataChar::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataI8::getRTTIStatic()
	{
		return ScriptSerializableFieldDataI8RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataI8::getRTTI() const
	{
		return ScriptSerializableFieldDataI8::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataU8::getRTTIStatic()
	{
		return ScriptSerializableFieldDataU8RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataU8::getRTTI() const
	{
		return ScriptSerializableFieldDataU8::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataI16::getRTTIStatic()
	{
		return ScriptSerializableFieldDataI16RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataI16::getRTTI() const
	{
		return ScriptSerializableFieldDataI16::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataU16::getRTTIStatic()
	{
		return ScriptSerializableFieldDataU16RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataU16::getRTTI() const
	{
		return ScriptSerializableFieldDataU16::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataI32::getRTTIStatic()
	{
		return ScriptSerializableFieldDataI32RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataI32::getRTTI() const
	{
		return ScriptSerializableFieldDataI32::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataU32::getRTTIStatic()
	{
		return ScriptSerializableFieldDataU32RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataU32::getRTTI() const
	{
		return ScriptSerializableFieldDataU32::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataI64::getRTTIStatic()
	{
		return ScriptSerializableFieldDataI64RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataI64::getRTTI() const
	{
		return ScriptSerializableFieldDataI64::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataU64::getRTTIStatic()
	{
		return ScriptSerializableFieldDataU64RTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataU64::getRTTI() const
	{
		return ScriptSerializableFieldDataU64::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataFloat::getRTTIStatic()
	{
		return ScriptSerializableFieldDataFloatRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataFloat::getRTTI() const
	{
		return ScriptSerializableFieldDataFloat::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataDouble::getRTTIStatic()
	{
		return ScriptSerializableFieldDataDoubleRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataDouble::getRTTI() const
	{
		return ScriptSerializableFieldDataDouble::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataString::getRTTIStatic()
	{
		return ScriptSerializableFieldDataStringRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataString::getRTTI() const
	{
		return ScriptSerializableFieldDataString::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataResourceRef::getRTTIStatic()
	{
		return ScriptSerializableFieldDataResourceRefRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataResourceRef::getRTTI() const
	{
		return ScriptSerializableFieldDataResourceRef::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataGameObjectRef::getRTTIStatic()
	{
		return ScriptSerializableFieldDataGameObjectRefRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataGameObjectRef::getRTTI() const
	{
		return ScriptSerializableFieldDataGameObjectRef::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataObject::getRTTIStatic()
	{
		return ScriptSerializableFieldDataObjectRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataObject::getRTTI() const
	{
		return ScriptSerializableFieldDataObject::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataArray::getRTTIStatic()
	{
		return ScriptSerializableFieldDataArrayRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataArray::getRTTI() const
	{
		return ScriptSerializableFieldDataArray::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataList::getRTTIStatic()
	{
		return ScriptSerializableFieldDataListRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataList::getRTTI() const
	{
		return ScriptSerializableFieldDataList::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableFieldDataDictionary::getRTTIStatic()
	{
		return ScriptSerializableFieldDataDictionaryRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldDataDictionary::getRTTI() const
	{
		return ScriptSerializableFieldDataDictionary::getRTTIStatic();
	}
}