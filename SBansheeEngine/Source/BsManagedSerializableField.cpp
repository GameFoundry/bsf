//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsManagedSerializableField.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableFieldRTTI.h"
#include "BsMonoUtil.h"
#include "BsMonoManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptTexture3D.h"
#include "BsScriptTextureCube.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptManagedResource.h"
#include "BsScriptPlainText.h"
#include "BsScriptScriptCode.h"
#include "BsScriptShader.h"
#include "BsScriptShaderInclude.h"
#include "BsScriptMaterial.h"
#include "BsScriptMesh.h"
#include "BsScriptPrefab.h"
#include "BsScriptFont.h"
#include "BsScriptStringTable.h"
#include "BsScriptGUISkin.h"
#include "BsScriptPhysicsMaterial.h"
#include "BsScriptPhysicsMesh.h"
#include "BsScriptSceneObject.h"
#include "BsScriptComponent.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableArray.h"
#include "BsManagedSerializableList.h"
#include "BsManagedSerializableDictionary.h"

namespace BansheeEngine
{
	template<class T>
	bool compareFieldData(const T* a, const ManagedSerializableFieldDataPtr& b)
	{
		if (rtti_is_of_type<T>(b))
		{
			auto castObj = std::static_pointer_cast<T>(b);
			return a->value == castObj->value;
		}

		return false;
	}

	ManagedSerializableFieldKey::ManagedSerializableFieldKey()
		:mTypeId(0), mFieldId(0)
	{ }

	ManagedSerializableFieldKey::ManagedSerializableFieldKey(UINT16 typeId, UINT16 fieldId)
		:mTypeId(typeId), mFieldId(fieldId)
	{ }

	ManagedSerializableFieldKeyPtr ManagedSerializableFieldKey::create(UINT16 typeId, UINT16 fieldId)
	{
		ManagedSerializableFieldKeyPtr fieldKey = bs_shared_ptr_new<ManagedSerializableFieldKey>(typeId, fieldId);
		return fieldKey;
	}

	ManagedSerializableFieldDataEntryPtr ManagedSerializableFieldDataEntry::create(const ManagedSerializableFieldKeyPtr& key, const ManagedSerializableFieldDataPtr& value)
	{
		ManagedSerializableFieldDataEntryPtr fieldDataEntry = bs_shared_ptr_new<ManagedSerializableFieldDataEntry>();
		fieldDataEntry->mKey = key;
		fieldDataEntry->mValue = value;

		return fieldDataEntry;
	}

	ManagedSerializableFieldDataPtr ManagedSerializableFieldData::create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* value)
	{
		return create(typeInfo, value, true);
	}

	ManagedSerializableFieldDataPtr ManagedSerializableFieldData::createDefault(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		return create(typeInfo, nullptr, false);
	}

	template<class ResType, class ScriptType>
	MonoObject* getScriptResource(const HResource& value)
	{
		ResourceHandle<ResType> castValue = static_resource_cast<ResType>(value);
		if (castValue.isLoaded())
		{
			ScriptType* scriptResource;
			ScriptResourceManager::instance().getScriptResource(castValue, &scriptResource, true);

			return scriptResource->getManagedInstance();
		}
		else
			return nullptr;
	}

	template<class ScriptType>
	ManagedSerializableFieldDataPtr setScriptResource(MonoObject* value)
	{
		auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataResourceRef>();

		if (value != nullptr)
		{
			ScriptType* scriptResource = ScriptType::toNative(value);
			fieldData->value = scriptResource->getHandle();
		}

		return fieldData;
	}

	struct ResourceFieldDataAccessors
	{
		std::function<MonoObject*(const HResource&)> getter;
		std::function<ManagedSerializableFieldDataPtr(MonoObject*)> setter;
	};

	ResourceFieldDataAccessors* getResourceFieldLookup()
	{
		static ResourceFieldDataAccessors lookup[(int)ScriptPrimitiveType::Count];
		static bool initialized = false;

		if(!initialized)
		{
			lookup[(int)ScriptPrimitiveType::Texture2DRef] =
				{ &getScriptResource<Texture, ScriptTexture2D>, &setScriptResource<ScriptTexture2D> };

			lookup[(int)ScriptPrimitiveType::Texture3DRef] =
				{ &getScriptResource<Texture, ScriptTexture3D>, &setScriptResource<ScriptTexture3D> };

			lookup[(int)ScriptPrimitiveType::TextureCubeRef] =
				{ &getScriptResource<Texture, ScriptTextureCube>, &setScriptResource<ScriptTextureCube> };

			lookup[(int)ScriptPrimitiveType::SpriteTextureRef] =
				{ &getScriptResource<SpriteTexture, ScriptSpriteTexture>, &setScriptResource<ScriptSpriteTexture> };

			lookup[(int)ScriptPrimitiveType::ShaderRef] =
				{ &getScriptResource<Shader, ScriptShader>, &setScriptResource<ScriptShader> };

			lookup[(int)ScriptPrimitiveType::ShaderIncludeRef] =
				{ &getScriptResource<ShaderInclude, ScriptShaderInclude>, &setScriptResource<ScriptShaderInclude> };

			lookup[(int)ScriptPrimitiveType::MaterialRef] =
				{ &getScriptResource<Material, ScriptMaterial>, &setScriptResource<ScriptMaterial> };

			lookup[(int)ScriptPrimitiveType::MeshRef] =
				{ &getScriptResource<Mesh, ScriptMesh>, &setScriptResource<ScriptMesh> };

			lookup[(int)ScriptPrimitiveType::PrefabRef] =
				{ &getScriptResource<Prefab, ScriptPrefab>, &setScriptResource<ScriptPrefab> };

			lookup[(int)ScriptPrimitiveType::FontRef] =
				{ &getScriptResource<Font, ScriptFont>, &setScriptResource<ScriptFont> };

			lookup[(int)ScriptPrimitiveType::StringTableRef] =
				{ &getScriptResource<StringTable, ScriptStringTable>, &setScriptResource<ScriptStringTable> };

			lookup[(int)ScriptPrimitiveType::GUISkinRef] =
				{ &getScriptResource<GUISkin, ScriptGUISkin>, &setScriptResource<ScriptGUISkin> };

			lookup[(int)ScriptPrimitiveType::PhysicsMaterialRef] =
				{ &getScriptResource<PhysicsMaterial, ScriptPhysicsMaterial>, &setScriptResource<ScriptPhysicsMaterial> };

			lookup[(int)ScriptPrimitiveType::PhysicsMeshRef] =
				{ &getScriptResource<PhysicsMesh, ScriptPhysicsMesh>, &setScriptResource<ScriptPhysicsMesh> };

			lookup[(int)ScriptPrimitiveType::ManagedResourceRef] =
				{ &getScriptResource<ManagedResource, ScriptManagedResource>, &setScriptResource<ScriptManagedResource> };

			lookup[(int)ScriptPrimitiveType::PlainTextRef] =
				{ &getScriptResource<PlainText, ScriptPlainText>, &setScriptResource<ScriptPlainText> };

			lookup[(int)ScriptPrimitiveType::ScriptCodeRef] =
				{ &getScriptResource<ScriptCode, ScriptScriptCode>, &setScriptResource<ScriptScriptCode> };
		}

		return lookup;
	}

	ManagedSerializableFieldDataPtr ManagedSerializableFieldData::create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* value, bool allowNull)
	{
		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);
			switch (primitiveTypeInfo->mType)
			{
			case ScriptPrimitiveType::Bool:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataBool>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Char:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataChar>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I8:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U8:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU8>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I16:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U16:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU16>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I32:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U32:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU32>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::I64:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataI64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::U64:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataU64>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Float:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataFloat>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

					return fieldData;
				}
			case ScriptPrimitiveType::Double:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataDouble>();
					if(value != nullptr)
						memcpy(&fieldData->value, mono_object_unbox(value), sizeof(fieldData->value));

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
			case ScriptPrimitiveType::SceneObjectRef:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataGameObjectRef>();

					if(value != nullptr)
					{
						ScriptSceneObject* scriptSceneObject = ScriptSceneObject::toNative(value);
						fieldData->value = static_object_cast<SceneObject>(scriptSceneObject->getNativeHandle());
					}

					return fieldData;
				}
			case ScriptPrimitiveType::ComponentRef:
				{
					auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataGameObjectRef>();

					if(value != nullptr)
					{
						ScriptComponent* scriptComponent = ScriptComponent::toNative(value);
						fieldData->value = static_object_cast<Component>(scriptComponent->getNativeHandle());
					}

					return fieldData;
				}
			default:
				// Must be a resource
				return getResourceFieldLookup()[(int)primitiveTypeInfo->mType].setter(value);
			}
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
			ManagedSerializableTypeInfoArrayPtr arrayTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(typeInfo);

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
			ManagedSerializableTypeInfoListPtr listTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoList>(typeInfo);

			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataList>();
			if(value != nullptr)
				fieldData->value = ManagedSerializableList::createFromExisting(value, listTypeInfo);
			else if (!allowNull)
				fieldData->value = ManagedSerializableList::createNew(listTypeInfo, 0);

			return fieldData;
		}
		else if(typeInfo->getTypeId() == TID_SerializableTypeInfoDictionary)
		{
			ManagedSerializableTypeInfoDictionaryPtr dictTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(typeInfo);

			auto fieldData = bs_shared_ptr_new<ManagedSerializableFieldDataDictionary>();
			if(value != nullptr)
				fieldData->value = ManagedSerializableDictionary::createFromExisting(value, dictTypeInfo);
			else if (!allowNull)
				fieldData->value = ManagedSerializableDictionary::createNew(dictTypeInfo);

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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
	}

	void* ManagedSerializableFieldDataString::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
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

	void* ManagedSerializableFieldDataResourceRef::getValue(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		static std::function<MonoObject*(const HResource&)> lookup[(int)ScriptPrimitiveType::Count];

		if(typeInfo->getTypeId() == TID_SerializableTypeInfoPrimitive)
		{
			auto primitiveTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoPrimitive>(typeInfo);

			return getResourceFieldLookup()[(int)primitiveTypeInfo->mType].getter(value);
		}

		BS_EXCEPT(InvalidParametersException, "Requesting an invalid type in serializable field.");
		return nullptr;
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
					ScriptSceneObject* scriptSceneObject = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(value);
					return scriptSceneObject->getManagedInstance();
				}
				else
					return nullptr;
			}
			else if(primitiveTypeInfo->mType == ScriptPrimitiveType::ComponentRef)
			{
				if (value)
				{
					ScriptComponent* scriptComponent = ScriptGameObjectManager::instance().getScriptComponent(value);
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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
		return nullptr;
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

	bool ManagedSerializableFieldDataBool::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataChar::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI8::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU8::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI16::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU16::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI32::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU32::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataI64::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataU64::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataFloat::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataDouble::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataString::equals(const ManagedSerializableFieldDataPtr& other)
	{
		if (rtti_is_of_type<ManagedSerializableFieldDataString>(other))
		{
			auto castObj = std::static_pointer_cast<ManagedSerializableFieldDataString>(other);
			return (isNull == true && isNull == castObj->isNull) || value == castObj->value;
		}

		return false;
	}

	bool ManagedSerializableFieldDataResourceRef::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataGameObjectRef::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataObject::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataArray::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataList::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	bool ManagedSerializableFieldDataDictionary::equals(const ManagedSerializableFieldDataPtr& other)
	{
		return compareFieldData(this, other);
	}

	size_t ManagedSerializableFieldDataBool::getHash()
	{
		std::hash<bool> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataChar::getHash()
	{
		std::hash<wchar_t> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataI8::getHash()
	{
		std::hash<INT8> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataU8::getHash()
	{
		std::hash<UINT8> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataI16::getHash()
	{
		std::hash<INT16> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataU16::getHash()
	{
		std::hash<UINT16> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataI32::getHash()
	{
		std::hash<INT32> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataU32::getHash()
	{
		std::hash<UINT32> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataI64::getHash()
	{
		std::hash<INT64> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataU64::getHash()
	{
		std::hash<UINT64> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataFloat::getHash()
	{
		std::hash<float> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataDouble::getHash()
	{
		std::hash<double> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataString::getHash()
	{
		std::hash<WString> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataResourceRef::getHash()
	{
		std::hash<String> hasher;
		return hasher(value.getUUID());
	}

	size_t ManagedSerializableFieldDataGameObjectRef::getHash()
	{
		std::hash<UINT64> hasher;
		return hasher(value.getInstanceId());
	}

	size_t ManagedSerializableFieldDataObject::getHash()
	{
		std::hash<std::shared_ptr<ManagedSerializableObject>> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataArray::getHash()
	{
		std::hash<std::shared_ptr<ManagedSerializableArray>> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataList::getHash()
	{
		std::hash<std::shared_ptr<ManagedSerializableList>> hasher;
		return hasher(value);
	}

	size_t ManagedSerializableFieldDataDictionary::getHash()
	{
		std::hash<std::shared_ptr<ManagedSerializableDictionary>> hasher;
		return hasher(value);
	}

	void ManagedSerializableFieldDataObject::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataObject::deserialize()
	{
		if (value != nullptr)
			value->deserialize();
	}

	void ManagedSerializableFieldDataArray::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataArray::deserialize()
	{
		if (value != nullptr)
			value->deserialize();
	}

	void ManagedSerializableFieldDataList::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataList::deserialize()
	{
		if (value != nullptr)
			value->deserialize();
	}

	void ManagedSerializableFieldDataDictionary::serialize()
	{
		if (value != nullptr)
			value->serialize();
	}

	void ManagedSerializableFieldDataDictionary::deserialize()
	{
		if (value != nullptr)
			value->deserialize();
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