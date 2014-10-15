#include "BsManagedSerializableField.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableFieldRTTI.h"
#include "BsMonoUtil.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptManagedResource.h"
#include "BsScriptSceneObject.h"
#include "BsScriptComponent.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableArray.h"
#include "BsManagedSerializableList.h"
#include "BsManagedSerializableDictionary.h"

namespace BansheeEngine
{
	ManagedSerializableFieldKeyPtr ManagedSerializableFieldKey::create(UINT16 typeId, UINT16 fieldId)
	{
		ManagedSerializableFieldKeyPtr fieldKey = bs_shared_ptr<ManagedSerializableFieldKey>();
		fieldKey->mTypeId = typeId;
		fieldKey->mFieldId = fieldId;

		return fieldKey;
	}

	ManagedSerializableFieldDataEntryPtr ManagedSerializableFieldDataEntry::create(const ManagedSerializableFieldKeyPtr& key, const ManagedSerializableFieldDataPtr& value)
	{
		ManagedSerializableFieldDataEntryPtr fieldDataEntry = bs_shared_ptr<ManagedSerializableFieldDataEntry>();
		fieldDataEntry->mKey = key;
		fieldDataEntry->mValue = value;

		return fieldDataEntry;
	}

	ManagedSerializableFieldDataPtr ManagedSerializableFieldData::create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* value)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			switch (primitiveTypeInfo->mType)
			{
			case ScriptPrimitiveType::Bool:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataBool>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Char:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataChar>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I8:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataI8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U8:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataU8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I16:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataI16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U16:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataU16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I32:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataI32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U32:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataU32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I64:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataI64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U64:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataU64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Float:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataFloat>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Double:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataDouble>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::String:
				{
					MonoString* strVal = (MonoString*)(value);

					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataString>();
					if(strVal != nullptr)
						fieldData->value = MonoUtil::monoToWString(strVal);
					return fieldData;
				}
			case ScriptPrimitiveType::TextureRef:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataResourceRef>();

					if(value != nullptr)
					{
						ScriptTexture2D* scriptTexture2D = ScriptTexture2D::toNative(value);
						fieldData->value = static_resource_cast<ScriptTexture2D>(scriptTexture2D->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SpriteTextureRef:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataResourceRef>();
					
					if(value != nullptr)
					{
						ScriptSpriteTexture* scriptSpriteTexture = ScriptSpriteTexture::toNative(value);
						fieldData->value = static_resource_cast<SpriteTexture>(scriptSpriteTexture->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::ManagedResourceRef:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataResourceRef>();
					
					if(value != nullptr)
					{
						ScriptManagedResource* scriptManagedResource = ScriptManagedResource::toNative(value);
						fieldData->value = static_resource_cast<ManagedResource>(scriptManagedResource->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SceneObjectRef:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataGameObjectRef>();

					if(value != nullptr)
					{
						ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(value);
						fieldData->value = static_object_cast<SceneObject>(scriptSceneObject->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::ComponentRef:
				{
					auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataGameObjectRef>();

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
			auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataObject>();
			if(value != nullptr)
			{
				fieldData->value = ManagedSerializableObject::createFromExisting(value);
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataArray>();
			if(value != nullptr)
			{
				fieldData->value = ManagedSerializableArray::createFromExisting(value, std::static_pointer_cast<ManagedSerializableTypeInfoArray>(typeInfo));
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataList>();
			if(value != nullptr)
			{
				fieldData->value = ManagedSerializableList::createFromExisting(value, std::static_pointer_cast<ManagedSerializableTypeInfoList>(typeInfo));
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			auto fieldData = bs_shared_ptr<ManagedSerializableFieldDataDictionary>();
			if(value != nullptr)
			{
				fieldData->value = ManagedSerializableDictionary::createFromExisting(value, std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(typeInfo));
			}

			return fieldData;
		}

		return nullptr;
	}

	void* ManagedSerializableFieldDataBool::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Bool)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataChar::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Char)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataI8::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I8)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataU8::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U8)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataI16::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I16)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataU16::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U16)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataI32::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I32)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataU32::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U32)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataI64::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::I64)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataU64::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::U64)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataFloat::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Float)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataDouble::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Double)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataString::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::String)
			{
				return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), value);
			}
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataResourceRef::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);

			if(primitiveTypeInfo->mType == ScriptPrimitiveType::TextureRef)
			{
				if(value)
				{
					HTexture texture = static_resource_cast<Texture>(value);
					ScriptTexture2D* scriptResource = ScriptResourceManager::instance().getScriptTexture(texture);
					if(scriptResource == nullptr)
						scriptResource = ScriptResourceManager::instance().createScriptTexture(texture);

					return scriptResource->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::SpriteTextureRef)
			{
				if(value)
				{
					HSpriteTexture spriteTexture = static_resource_cast<SpriteTexture>(value);
					ScriptSpriteTexture* scriptResource = ScriptResourceManager::instance().getScriptSpriteTexture(spriteTexture);
					if(scriptResource == nullptr)
						scriptResource = ScriptResourceManager::instance().createScriptSpriteTexture(spriteTexture);

					if(scriptResource != nullptr)
						return scriptResource->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if (primitiveTypeInfo->mType == ScriptPrimitiveType::ManagedResourceRef)
			{
				if (value)
				{
					HManagedResource managedResource = static_resource_cast<ManagedResource>(value);
					ScriptManagedResource* scriptResource = ScriptResourceManager::instance().getScriptManagedResource(managedResource);
					assert(scriptResource != nullptr); // Managed resource managed instance is created upon creation so it may never be null

					return scriptResource->getManagedInstance();
				}
				else
					return nullptr;
			}
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataGameObjectRef::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);

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

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataObject::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoObject>(typeInfo);

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

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataArray::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataList::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			auto listTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoList>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ManagedSerializableFieldDataDictionary::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			auto dictionaryTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataBool::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Bool)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_boolean_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataChar::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Char)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_char_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataI8::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::I8)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_sbyte_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataU8::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::U8)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_byte_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataI16::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::I16)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_int16_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataU16::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::U16)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_uint16_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataI32::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::I32)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_int32_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataU32::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::U32)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_uint32_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataI64::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::I64)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_int64_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataU64::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::U64)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_uint64_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataFloat::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Float)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_single_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataDouble::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Double)
				return mono_value_box(MonoManager::instance().getDomain(), mono_get_double_class(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataString::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataResourceRef::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataGameObjectRef::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataObject::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoObject>(typeInfo);

			if (value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	MonoObject* ManagedSerializableFieldDataArray::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataList::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataDictionary::getValueBoxed(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	RTTITypeBase* ManagedSerializableFieldKey::getRTTIStatic()
	{
		return ManagedSerializableFieldKeyRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldKey::getRTTI() const
	{
		return ManagedSerializableFieldKey::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldData::getRTTIStatic()
	{
		return ManagedSerializableFieldDataRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldData::getRTTI() const
	{
		return ManagedSerializableFieldData::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataEntry::getRTTIStatic()
	{
		return ManagedSerializableFieldDataEntryRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataEntry::getRTTI() const
	{
		return ManagedSerializableFieldDataEntry::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataBool::getRTTIStatic()
	{
		return ManagedSerializableFieldDataBoolRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataBool::getRTTI() const
	{
		return ManagedSerializableFieldDataBool::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataChar::getRTTIStatic()
	{
		return ManagedSerializableFieldDataCharRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataChar::getRTTI() const
	{
		return ManagedSerializableFieldDataChar::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataI8::getRTTIStatic()
	{
		return ManagedSerializableFieldDataI8RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataI8::getRTTI() const
	{
		return ManagedSerializableFieldDataI8::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataU8::getRTTIStatic()
	{
		return ManagedSerializableFieldDataU8RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataU8::getRTTI() const
	{
		return ManagedSerializableFieldDataU8::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataI16::getRTTIStatic()
	{
		return ManagedSerializableFieldDataI16RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataI16::getRTTI() const
	{
		return ManagedSerializableFieldDataI16::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataU16::getRTTIStatic()
	{
		return ManagedSerializableFieldDataU16RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataU16::getRTTI() const
	{
		return ManagedSerializableFieldDataU16::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataI32::getRTTIStatic()
	{
		return ManagedSerializableFieldDataI32RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataI32::getRTTI() const
	{
		return ManagedSerializableFieldDataI32::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataU32::getRTTIStatic()
	{
		return ManagedSerializableFieldDataU32RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataU32::getRTTI() const
	{
		return ManagedSerializableFieldDataU32::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataI64::getRTTIStatic()
	{
		return ManagedSerializableFieldDataI64RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataI64::getRTTI() const
	{
		return ManagedSerializableFieldDataI64::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataU64::getRTTIStatic()
	{
		return ManagedSerializableFieldDataU64RTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataU64::getRTTI() const
	{
		return ManagedSerializableFieldDataU64::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataFloat::getRTTIStatic()
	{
		return ManagedSerializableFieldDataFloatRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataFloat::getRTTI() const
	{
		return ManagedSerializableFieldDataFloat::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataDouble::getRTTIStatic()
	{
		return ManagedSerializableFieldDataDoubleRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataDouble::getRTTI() const
	{
		return ManagedSerializableFieldDataDouble::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataString::getRTTIStatic()
	{
		return ManagedSerializableFieldDataStringRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataString::getRTTI() const
	{
		return ManagedSerializableFieldDataString::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataResourceRef::getRTTIStatic()
	{
		return ManagedSerializableFieldDataResourceRefRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataResourceRef::getRTTI() const
	{
		return ManagedSerializableFieldDataResourceRef::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataGameObjectRef::getRTTIStatic()
	{
		return ManagedSerializableFieldDataGameObjectRefRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataGameObjectRef::getRTTI() const
	{
		return ManagedSerializableFieldDataGameObjectRef::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataObject::getRTTIStatic()
	{
		return ManagedSerializableFieldDataObjectRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataObject::getRTTI() const
	{
		return ManagedSerializableFieldDataObject::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataArray::getRTTIStatic()
	{
		return ManagedSerializableFieldDataArrayRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataArray::getRTTI() const
	{
		return ManagedSerializableFieldDataArray::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataList::getRTTIStatic()
	{
		return ManagedSerializableFieldDataListRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataList::getRTTI() const
	{
		return ManagedSerializableFieldDataList::getRTTIStatic();
	}

	RTTITypeBase* ManagedSerializableFieldDataDictionary::getRTTIStatic()
	{
		return ManagedSerializableFieldDataDictionaryRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataDictionary::getRTTI() const
	{
		return ManagedSerializableFieldDataDictionary::getRTTIStatic();
	}
}