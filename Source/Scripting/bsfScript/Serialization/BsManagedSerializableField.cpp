//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Serialization/BsManagedSerializableField.h"
#include "Serialization/BsManagedSerializableObjectInfo.h"
#include "RTTI/BsManagedSerializableFieldRTTI.h"
#include "BsMonoUtil.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "Wrappers/BsScriptManagedResource.h"
#include "Wrappers/BsScriptSceneObject.h"
#include "Wrappers/BsScriptComponent.h"
#include "Wrappers/BsScriptManagedComponent.h"
#include "Serialization/BsManagedSerializableObject.h"
#include "Serialization/BsManagedSerializableArray.h"
#include "Serialization/BsManagedSerializableList.h"
#include "Serialization/BsManagedSerializableDictionary.h"
#include "Serialization/BsScriptAssemblyManager.h"
#include "Wrappers/BsScriptReflectable.h"

namespace bs
{
	template<class T>
	bool compareFieldData(const T* a, const SPtr<ManagedSerializableFieldData>& b)
	{
		if (rtti_is_of_type<T>(b))
		{
			auto castObj = std::static_pointer_cast<T>(b);
			return a->value == castObj->value;
		}

		return false;
	}

	bool compareFieldData(const SPtr<ManagedSerializableFieldData>& oldData, const SPtr<ManagedSerializableFieldData>& newData)
	{
		if (!oldData)
			return !newData;
		else
		{
			if (!newData)
				return false;
		}

		return oldData->equals(newData);
	}

	bool isPrimitiveOrEnumType(const SPtr<ManagedSerializableTypeInfo>& typeInfo, ScriptPrimitiveType underlyingType)
	{
		if(const auto primitiveTypeInfo = rtti_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo.get()))
			return primitiveTypeInfo->mType == underlyingType;
		else if(const auto enumTypeInfo = rtti_cast<ManagedSerializableTypeInfoEnum>(typeInfo.get()))
			return enumTypeInfo->mUnderlyingType == underlyingType;

		return false;
	}

	ManagedSerializableFieldKey::ManagedSerializableFieldKey(UINT16 typeId, UINT16 fieldId)
		:mTypeId(typeId), mFieldId(fieldId)
	{ }

	SPtr<ManagedSerializableFieldKey> ManagedSerializableFieldKey::create(UINT16 typeId, UINT16 fieldId)
	{
		SPtr<ManagedSerializableFieldKey> fieldKey = bs_shared_ptr_new<ManagedSerializableFieldKey>(typeId, fieldId);
		return fieldKey;
	}

	SPtr<ManagedSerializableFieldDataEntry> ManagedSerializableFieldDataEntry::create(const SPtr<ManagedSerializableFieldKey>& key, const SPtr<ManagedSerializableFieldData>& value)
	{
		SPtr<ManagedSerializableFieldDataEntry> fieldDataEntry = bs_shared_ptr_new<ManagedSerializableFieldDataEntry>();
		fieldDataEntry->mKey = key;
		fieldDataEntry->mValue = value;

		return fieldDataEntry;
	}

	SPtr<ManagedSerializableFieldData> ManagedSerializableFieldData::create(const SPtr<ManagedSerializableTypeInfo>& typeInfo, MonoObject* value)
	{
		return create(typeInfo, value, true);
	}

	SPtr<ManagedSerializableFieldData> ManagedSerializableFieldData::createDefault(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return create(typeInfo, nullptr, false);
	}

	SPtr<ManagedSerializableFieldData> ManagedSerializableFieldData::create(const SPtr<ManagedSerializableTypeInfo>& typeInfo, MonoObject* value, bool allowNull)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive || typeInfo->getTypeId() == TID_SerializableTypeInfoEnum)
		{
			ScriptPrimitiveType primitiveType = ScriptPrimitiveType::I32;

			if(auto primitiveTypeInfo = rtti_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo.get()))
				primitiveType = primitiveTypeInfo->mType;
			else if(auto enumTypeInfo = rtti_cast<ManagedSerializableTypeInfoEnum>(typeInfo.get()))
				primitiveType = enumTypeInfo->mUnderlyingType;

			switch (primitiveType)
			{
			case ScriptPrimitiveType::Bool:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataBool>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Char:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataChar>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I8:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI8>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U8:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU8>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I16:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI16>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U16:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU16>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I32:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI32>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U32:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU32>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I64:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI64>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U64:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU64>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Float:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataFloat>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Double:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataDouble>();
					if(value != nullptr)
						memcpy(&fieldData->value, MonoUtil::unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::String:
				{
					MonoString* strVal = (MonoString*)(value);

					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataString>();
					if (strVal != nullptr)
						fieldData->value = MonoUtil::monoToWString(strVal);
					else
						fieldData->isNull = allowNull;

					return fieldData;
				}
			default:
				break;
			}
		}
		else if (typeInfo->getTypeId() == TID_SerializableTypeInfoRef)
		{
			auto refTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoRef>(typeInfo);
			switch (refTypeInfo->mType)
			{
			case ScriptReferenceType::SceneObject:
			{
				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataGameObjectRef>();

				if (value != nullptr)
				{
					ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(value);
					fieldData->value = scriptSceneObject->getNativeHandle();
				}

				return fieldData;
			}
			case ScriptReferenceType::ManagedComponentBase:
			case ScriptReferenceType::ManagedComponent:
			{
				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataGameObjectRef>();

				if (value != nullptr)
				{
					ScriptManagedComponent* scriptComponent = ScriptManagedComponent::toNative(value);
					fieldData->value = scriptComponent->getNativeHandle();
				}

				return fieldData;
			}
			case ScriptReferenceType::BuiltinComponentBase:
			case ScriptReferenceType::BuiltinComponent:
			{
				BuiltinComponentInfo* info = ScriptAssemblyManager::instance().getBuiltinComponentInfo(refTypeInfo->mRTIITypeId);
				if (info == nullptr)
					return nullptr;

				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataGameObjectRef>();

				if (value != nullptr)
				{
					ScriptComponentBase* scriptComponent = ScriptComponent::toNative(value);
					fieldData->value = static_object_cast<GameObject>(scriptComponent->getComponent());
				}

				return fieldData;
			}
			case ScriptReferenceType::ManagedResourceBase:
			case ScriptReferenceType::ManagedResource:
			{
				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataResourceRef>();

				if (value != nullptr)
				{
					ScriptResourceBase* scriptResource = ScriptManagedResource::toNative(value);
					fieldData->value = scriptResource->getGenericHandle();
				}

				return fieldData;
			}
			case ScriptReferenceType::BuiltinResourceBase:
			case ScriptReferenceType::BuiltinResource:
			{
				BuiltinResourceInfo* info = ScriptAssemblyManager::instance().getBuiltinResourceInfo(refTypeInfo->mRTIITypeId);
				if (info == nullptr)
					return nullptr;

				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataResourceRef>();

				if (value != nullptr)
				{
					ScriptResourceBase* scriptResource = ScriptResource::toNative(value);
					fieldData->value = scriptResource->getGenericHandle();
				}

				return fieldData;
			}
			case ScriptReferenceType::ReflectableObject:
			{
				ReflectableTypeInfo* info = ScriptAssemblyManager::instance().getReflectableTypeInfo(refTypeInfo->mRTIITypeId);
				if (info == nullptr)
					return nullptr;

				auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataReflectableRef>();

				if (value != nullptr)
				{
					ScriptReflectableBase* scriptReflectable = (ScriptReflectableBase*)ScriptObjectImpl::toNative(value);
					fieldData->value = scriptReflectable->getReflectable();
				}

				return fieldData;
			}
			default:
				break;
			}
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoRRef)
		{
			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataResourceRef>();

			if(value != nullptr)
			{
				ScriptRRefBase* scriptRRefBase = ScriptRRefBase::toNative(value);
				fieldData->value = scriptRRefBase->getHandle();
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataObject>();
			if (value != nullptr)
				fieldData->value = ManagedSerializableObject::createFromExisting(value);
			else if (!allowNull)
				fieldData->value = ManagedSerializableObject::createNew(std::static_pointer_cast<ManagedSerializableTypeInfoObject>(typeInfo));

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			SPtr<ManagedSerializableTypeInfoArray> arrayTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(typeInfo);

			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataArray>();
			if(value != nullptr)
				fieldData->value = ManagedSerializableArray::createFromExisting(value, arrayTypeInfo);
			else if (!allowNull)
			{
				Vector<UINT32> sizes(arrayTypeInfo->mRank, 0);
				fieldData->value = ManagedSerializableArray::createNew(arrayTypeInfo, sizes);
			}

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			SPtr<ManagedSerializableTypeInfoList> listTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoList>(typeInfo);

			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataList>();
			if(value != nullptr)
				fieldData->value = ManagedSerializableList::createFromExisting(value, listTypeInfo);
			else if (!allowNull)
				fieldData->value = ManagedSerializableList::createNew(listTypeInfo, 0);

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			SPtr<ManagedSerializableTypeInfoDictionary> dictTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(typeInfo);

			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataDictionary>();
			if(value != nullptr)
				fieldData->value = ManagedSerializableDictionary::createFromExisting(value, dictTypeInfo);
			else if (!allowNull)
				fieldData->value = ManagedSerializableDictionary::createNew(dictTypeInfo);

			return fieldData;
		}

		return nullptr;
	}

	void* ManagedSerializableFieldDataBool::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::Bool))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataChar::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::Char))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataI8::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I8))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataU8::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U8))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataI16::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I16))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataU16::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U16))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataI32::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I32))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataU32::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U32))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataI64::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I64))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataU64::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U64))
			return &value;

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataFloat::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Float)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataDouble::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::Double)
				return &value;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataString::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if(primitiveTypeInfo->mType == ScriptPrimitiveType::String)
			{
				if (!isNull)
					return MonoUtil::wstringToMono(value);
				else
					return nullptr;
			}
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataResourceRef::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoRef)
		{
			const auto refTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoRef>(typeInfo);

			if (!value.isLoaded())
				return nullptr;

			if (refTypeInfo->mType == ScriptReferenceType::ManagedResourceBase ||
				refTypeInfo->mType == ScriptReferenceType::ManagedResource)
			{
				ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(value, false);
				assert(scriptResource != nullptr);

				return scriptResource->getManagedInstance();
			}
			else if (refTypeInfo->mType == ScriptReferenceType::BuiltinResourceBase ||
					 refTypeInfo->mType == ScriptReferenceType::BuiltinResource)
			{
				ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(value, true);

				return scriptResource->getManagedInstance();
			}
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoRRef)
		{
			const auto refTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoRRef>(typeInfo);

			::MonoClass* resourceRRefClass = nullptr;
			if(refTypeInfo->mResourceType)
			{
				if (!typeInfo->isTypeLoaded())
					return nullptr;

				resourceRRefClass = typeInfo->getMonoClass();
				if (resourceRRefClass == nullptr)
					return nullptr;
			}

			// Note: Each reference ref ends up creating its own object instance. Perhaps share the same instance between
			// all references to the same resource?

			return ScriptRRefBase::create(value, resourceRRefClass);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataGameObjectRef::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoRef)
		{
			auto refTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoRef>(typeInfo);

			if(refTypeInfo->mType == ScriptReferenceType::SceneObject)
			{
				if(value)
				{
					ScriptSceneObject* scriptSceneObject =
						ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(static_object_cast<SceneObject>(value));
					return scriptSceneObject->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if(refTypeInfo->mType == ScriptReferenceType::ManagedComponentBase ||
					refTypeInfo->mType == ScriptReferenceType::ManagedComponent)
			{
				if (value)
				{
					ScriptManagedComponent* scriptComponent =
						ScriptGameObjectManager::instance().getManagedScriptComponent(static_object_cast<ManagedComponent>(value));
					assert(scriptComponent != nullptr);

					return scriptComponent->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if (refTypeInfo->mType == ScriptReferenceType::BuiltinComponentBase ||
					 refTypeInfo->mType == ScriptReferenceType::BuiltinComponent)
			{
				if (value)
				{
					ScriptComponentBase* scriptComponent =
						ScriptGameObjectManager::instance().getBuiltinScriptComponent(static_object_cast<Component>(value));
					assert(scriptComponent != nullptr);

					return scriptComponent->getManagedInstance();
				}
				else
					return nullptr;
			}
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataReflectableRef::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoRef)
		{
			const auto refTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoRef>(typeInfo);

			if (!value)
				return nullptr;

			UINT32 rttiId = refTypeInfo->mRTIITypeId;
			ReflectableTypeInfo* info = ScriptAssemblyManager::instance().getReflectableTypeInfo(rttiId);

			if (info == nullptr)
				return nullptr;

			return info->createCallback(value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataObject::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
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
						return MonoUtil::unbox(managedInstance); // Structs are passed as raw types because mono expects them as such
				}
				else
					return value->getManagedInstance();
			}

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataArray::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoArray)
		{
			auto objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataList::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoList)
		{
			auto listTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoList>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	void* ManagedSerializableFieldDataDictionary::getValue(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			auto dictionaryTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(typeInfo);

			if(value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataBool::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::Bool))
			return MonoUtil::box(MonoUtil::getBoolClass(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataChar::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::Char))
			return MonoUtil::box(MonoUtil::getCharClass(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataI8::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I8))
			return MonoUtil::box(MonoUtil::getSByteClass(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataU8::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U8))
			return MonoUtil::box(MonoUtil::getByteClass(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataI16::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I16))
			return MonoUtil::box(MonoUtil::getINT16Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataU16::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U16))
			return MonoUtil::box(MonoUtil::getUINT16Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataI32::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I32))
			return MonoUtil::box(MonoUtil::getINT32Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataU32::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U32))
			return MonoUtil::box(MonoUtil::getUINT32Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataI64::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::I64))
			return MonoUtil::box(MonoUtil::getINT64Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataU64::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if(isPrimitiveOrEnumType(typeInfo, ScriptPrimitiveType::U64))
			return MonoUtil::box(MonoUtil::getUINT64Class(), &value);

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataFloat::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Float)
				return MonoUtil::box(MonoUtil::getFloatClass(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataDouble::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			if (primitiveTypeInfo->mType == ScriptPrimitiveType::Double)
				return MonoUtil::box(MonoUtil::getDoubleClass(), &value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataString::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataResourceRef::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataGameObjectRef::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataReflectableRef::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataObject::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		if (typeInfo->getTypeId() == TID_SerializableTypeInfoObject)
		{
			auto objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoObject>(typeInfo);

			if (value != nullptr)
				return value->getManagedInstance();

			return nullptr;
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
	}

	MonoObject* ManagedSerializableFieldDataArray::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataList::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	MonoObject* ManagedSerializableFieldDataDictionary::getValueBoxed(const SPtr<ManagedSerializableTypeInfo>& typeInfo)
	{
		return (MonoObject*)getValue(typeInfo);
	}

	bool ManagedSerializableFieldDataBool::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataChar::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI8::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU8::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI16::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU16::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI32::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU32::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI64::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU64::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataFloat::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataDouble::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataString::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		if (rtti_is_of_type<ManagedSerializableFieldDataString>(other))
		{
			auto castObj = std::static_pointer_cast<ManagedSerializableFieldDataString>(other);
			return (isNull == true && isNull == castObj->isNull) || value == castObj->value;
		}

		return false;
	}

	bool ManagedSerializableFieldDataResourceRef::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataGameObjectRef::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataReflectableRef::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataObject::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		if (auto otherObj = rtti_cast<ManagedSerializableFieldDataObject>(other))
		{
			if(!value && !otherObj->value)
				return true;


			if((value == nullptr && otherObj->value) || (value && !otherObj->value))
				return false;

			return value->equals(*otherObj->value);
		}

		return false;
	}

	bool ManagedSerializableFieldDataArray::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		if (auto otherObj = rtti_cast<ManagedSerializableFieldDataArray>(other))
		{
			if(!value && !otherObj->value)
				return true;

			if((!value && otherObj->value) || (value && !otherObj->value))
				return false;

			UINT32 oldLength = value->getTotalLength();
			UINT32 newLength = otherObj->value->getTotalLength();

			if(oldLength != newLength)
				return false;

			for (UINT32 i = 0; i < newLength; i++)
			{
				SPtr<ManagedSerializableFieldData> oldData = value->getFieldData(i);
				SPtr<ManagedSerializableFieldData> newData = otherObj->value->getFieldData(i);

				if (compareFieldData(oldData, newData))
					return false;
			}

			return true;
		}

		return false;
	}

	bool ManagedSerializableFieldDataList::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		if (auto otherObj = rtti_cast<ManagedSerializableFieldDataList>(other))
		{
			if(!value && !otherObj->value)
				return true;

			if((!value && otherObj->value) || (value && !otherObj->value))
				return false;

			UINT32 oldLength = value->getLength();
			UINT32 newLength = otherObj->value->getLength();

			if(oldLength != newLength)
				return false;

			for (UINT32 i = 0; i < newLength; i++)
			{
				SPtr<ManagedSerializableFieldData> oldData = value->getFieldData(i);
				SPtr<ManagedSerializableFieldData> newData = otherObj->value->getFieldData(i);

				if (compareFieldData(oldData, newData))
					return false;
			}

			return true;
		}

		return false;
	}

	bool ManagedSerializableFieldDataDictionary::equals(const SPtr<ManagedSerializableFieldData>& other)
	{
		if (auto otherObj = rtti_cast<ManagedSerializableFieldDataDictionary>(other))
		{
			if(!value && !otherObj->value)
				return true;

			if((!value && otherObj->value) || (value && !otherObj->value))
				return false;

			auto newEnumerator = otherObj->value->getEnumerator();
			while (newEnumerator.moveNext())
			{
				SPtr<ManagedSerializableFieldData> key = newEnumerator.getKey();
				if (value->contains(key))
				{
					if(!compareFieldData(value->getFieldData(key), newEnumerator.getValue()))
						return false;
				}
				else
					return false;
			}

			auto oldEnumerator = value->getEnumerator();
			while (oldEnumerator.moveNext())
			{
				SPtr<ManagedSerializableFieldData> key = oldEnumerator.getKey();
				if (!otherObj->value->contains(oldEnumerator.getKey()))
					return false;
			}

			return true;
		}

		return false;;
	}

	size_t ManagedSerializableFieldDataBool::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataChar::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataI8::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataU8::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataI16::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataU16::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataI32::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataU32::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataI64::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataU64::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataFloat::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataDouble::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataString::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataResourceRef::getHash()
	{
		return bs_hash(value.getUUID());
	}

	size_t ManagedSerializableFieldDataGameObjectRef::getHash()
	{
		return bs_hash(value.getInstanceId());
	}

	size_t ManagedSerializableFieldDataReflectableRef::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataObject::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataArray::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataList::getHash()
	{
		return bs_hash(value);
	}

	size_t ManagedSerializableFieldDataDictionary::getHash()
	{
		return bs_hash(value);
	}

	void ManagedSerializableFieldDataObject::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataObject::deserialize()
	{
		if (value != nullptr)
		{
			MonoObject* managedInstance = value->deserialize();
			value = ManagedSerializableObject::createFromExisting(managedInstance);
		}
	}

	void ManagedSerializableFieldDataArray::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataArray::deserialize()
	{
		if (value != nullptr)
		{
			MonoObject* managedInstance = value->deserialize();
			value = ManagedSerializableArray::createFromExisting(managedInstance, value->getTypeInfo());
		}
	}

	void ManagedSerializableFieldDataList::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataList::deserialize()
	{
		if (value != nullptr)
		{
			MonoObject* managedInstance = value->deserialize();
			value = ManagedSerializableList::createFromExisting(managedInstance, value->getTypeInfo());
		}
	}

	void ManagedSerializableFieldDataDictionary::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataDictionary::deserialize()
	{
		if (value != nullptr)
		{
			MonoObject* managedInstance = value->deserialize();
			value = ManagedSerializableDictionary::createFromExisting(managedInstance, value->getTypeInfo());
		}
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

	RTTITypeBase* ManagedSerializableFieldDataReflectableRef::getRTTIStatic()
	{
		return ManagedSerializableFieldDataReflectableRefRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableFieldDataReflectableRef::getRTTI() const
	{
		return ManagedSerializableFieldDataReflectableRef::getRTTIStatic();
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
