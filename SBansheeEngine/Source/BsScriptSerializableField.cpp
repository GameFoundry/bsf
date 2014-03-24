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

	ScriptSerializableFieldDataPtr ScriptSerializableFieldData::create(const ScriptSerializableTypeInfoPtr& typeInfo, void* value)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);
			switch (primitiveTypeInfo->mType)
			{
			case ScriptPrimitiveType::Bool:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataBool>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Char:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataChar>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I8:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI8>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U8:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU8>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I16:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI16>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U16:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU16>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I32:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI32>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U32:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU32>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::I64:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataI64>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::U64:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataU64>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Float:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataFloat>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::Double:
				{
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataDouble>();
					memcpy(&fieldData->value, value, sizeof(fieldData->value));
					return fieldData;
				}
			case ScriptPrimitiveType::String:
				{
					MonoString* strVal = *(MonoString**)(value);

					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataString>();
					if(strVal != nullptr)
						fieldData->value = MonoUtil::monoToWString(strVal);
					return fieldData;
				}
			case ScriptPrimitiveType::TextureRef:
				{
					MonoObject* objVal = *(MonoObject**)(value);
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();

					if(objVal != nullptr)
					{
						ScriptTexture2D* scriptTexture2D = ScriptTexture2D::toNative(objVal);
						fieldData->value = static_resource_cast<ScriptTexture2D>(scriptTexture2D->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SpriteTextureRef:
				{
					MonoObject* objVal = *(MonoObject**)(value);
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataResourceRef>();
					
					if(objVal != nullptr)
					{
						ScriptSpriteTexture* scriptSpriteTexture = ScriptSpriteTexture::toNative(objVal);
						fieldData->value = static_resource_cast<SpriteTexture>(scriptSpriteTexture->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::SceneObjectRef:
				{
					MonoObject* objVal = *(MonoObject**)(value);
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();

					if(objVal != nullptr)
					{
						ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(objVal);
						fieldData->value = static_object_cast<SceneObject>(scriptSceneObject->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::ComponentRef:
				{
					MonoObject* objVal = *(MonoObject**)(value);
					auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataGameObjectRef>();

					if(objVal != nullptr)
					{
						ScriptComponent* scriptComponent = ScriptComponent::toNative(objVal);
						fieldData->value = static_object_cast<Component>(scriptComponent->getNativeHandle());
					}

					return fieldData;
				}
			}
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			MonoObject* objVal = *(MonoObject**)(value);

			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataObject>();
			if(objVal != nullptr)
			{
				fieldData->value = ScriptSerializableObject::create(objVal);
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			MonoObject* objVal = *(MonoObject**)(value);

			auto fieldData = cm_shared_ptr<ScriptSerializableFieldDataArray>();
			if(objVal != nullptr)
			{
				fieldData->value = ScriptSerializableArray::create(objVal, std::static_pointer_cast<ScriptSerializableTypeInfoArray>(typeInfo));
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
				ScriptTexture2D* scriptResource = ScriptResourceManager::instance().getScriptTexture(value);
				if(scriptResource == nullptr)
					scriptResource = ScriptResourceManager::instance().createScriptTexture(value);

				return scriptResource->getManagedInstance();
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::SpriteTextureRef)
			{
				ScriptSpriteTexture* scriptResource = ScriptResourceManager::instance().getScriptSpriteTexture(value);
				if(scriptResource == nullptr)
					scriptResource = ScriptResourceManager::instance().createScriptSpriteTexture(value);

				return scriptResource->getManagedInstance();
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
				ScriptSceneObject* scriptSceneObject = ScriptGameObjectManager::instance().getScriptSceneObject(value);
				if(scriptSceneObject == nullptr)
					scriptSceneObject = ScriptGameObjectManager::instance().createScriptSceneObject(value);

				return scriptSceneObject->getManagedInstance();
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::ComponentRef)
			{
				ScriptComponent* scriptComponent = ScriptGameObjectManager::instance().getScriptComponent(value);
				if(scriptComponent == nullptr)
					scriptComponent = ScriptGameObjectManager::instance().createScriptComponent(value);

				return scriptComponent->getManagedInstance();
			}
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataObject::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto objectTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoObject>(typeInfo);

			return value->getManagedInstance();
		}

		CM_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
	}

	void* ScriptSerializableFieldDataArray::getValue(const ScriptSerializableTypeInfoPtr& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto objectTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoArray>(typeInfo);

			return value->getManagedInstance();
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
}