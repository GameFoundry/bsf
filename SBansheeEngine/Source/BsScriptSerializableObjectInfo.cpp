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
#include "BsScriptSerializableObjectInfo.h"
#include "BsRuntimeScriptObjects.h"

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
		:mMonoField(nullptr), mFlags((ScriptFieldFlags)0), mFieldId(0)
	{

	}

	RTTITypeBase* ScriptSerializableFieldInfo::getRTTIStatic()
	{
		return ScriptSerializableFieldInfoRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableFieldInfo::getRTTI() const
	{
		return ScriptSerializableFieldInfo::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableTypeInfo::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfo::getRTTI() const
	{
		return ScriptSerializableTypeInfo::getRTTIStatic();
	}

	bool ScriptSerializableTypeInfoPrimitive::matches(const ScriptSerializableTypeInfoPtr& typeInfo) const
	{
		if(!rtti_is_of_type<ScriptSerializableTypeInfoPrimitive>(typeInfo))
			return false;

		auto primTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoPrimitive>(typeInfo);

		return primTypeInfo->mType == mType;
	}

	bool ScriptSerializableTypeInfoPrimitive::isTypeLoaded() const
	{
		return true;
	}

	::MonoClass* ScriptSerializableTypeInfoPrimitive::getMonoClass() const
	{
		switch(mType)
		{
		case ScriptPrimitiveType::Bool:
			return mono_get_boolean_class();
		case ScriptPrimitiveType::Char:
			return mono_get_char_class();
		case ScriptPrimitiveType::I8:
			return mono_get_sbyte_class();
		case ScriptPrimitiveType::U8:
			return mono_get_byte_class();
		case ScriptPrimitiveType::I16:
			return mono_get_int16_class();
		case ScriptPrimitiveType::U16:
			return mono_get_uint16_class();
		case ScriptPrimitiveType::I32:
			return mono_get_int32_class();
		case ScriptPrimitiveType::U32:
			return mono_get_uint32_class();
		case ScriptPrimitiveType::I64:
			return mono_get_int64_class();
		case ScriptPrimitiveType::U64:
			return mono_get_uint64_class();
		case ScriptPrimitiveType::Float:
			return mono_get_single_class();
		case ScriptPrimitiveType::Double:
			return mono_get_double_class();
		case ScriptPrimitiveType::String:
			return mono_get_string_class();
		case ScriptPrimitiveType::TextureRef:
			return RuntimeScriptObjects::instance().getTextureClass()->_getInternalClass();
		case ScriptPrimitiveType::SpriteTextureRef:
			return RuntimeScriptObjects::instance().getSpriteTextureClass()->_getInternalClass();
		case ScriptPrimitiveType::SceneObjectRef:
			return RuntimeScriptObjects::instance().getSceneObjectClass()->_getInternalClass();
		case ScriptPrimitiveType::ComponentRef:
			return RuntimeScriptObjects::instance().getComponentClass()->_getInternalClass();
		}

		return nullptr;
	}

	RTTITypeBase* ScriptSerializableTypeInfoPrimitive::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoPrimitiveRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoPrimitive::getRTTI() const
	{
		return ScriptSerializableTypeInfoPrimitive::getRTTIStatic();
	}

	bool ScriptSerializableTypeInfoObject::matches(const ScriptSerializableTypeInfoPtr& typeInfo) const
	{
		if(!rtti_is_of_type<ScriptSerializableTypeInfoObject>(typeInfo))
			return false;

		auto objTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoObject>(typeInfo);

		return objTypeInfo->mTypeNamespace == mTypeNamespace && objTypeInfo->mTypeName == mTypeName && objTypeInfo->mValueType == mValueType;
	}

	bool ScriptSerializableTypeInfoObject::isTypeLoaded() const
	{
		return RuntimeScriptObjects::instance().hasSerializableObjectInfo(mTypeNamespace, mTypeName);
	}

	::MonoClass* ScriptSerializableTypeInfoObject::getMonoClass() const
	{
		ScriptSerializableObjectInfoPtr objInfo;
		if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(mTypeNamespace, mTypeName, objInfo))
			return nullptr;

		return objInfo->mMonoClass->_getInternalClass();
	}

	RTTITypeBase* ScriptSerializableTypeInfoObject::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoObjectRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoObject::getRTTI() const
	{
		return ScriptSerializableTypeInfoObject::getRTTIStatic();
	}

	bool ScriptSerializableTypeInfoArray::matches(const ScriptSerializableTypeInfoPtr& typeInfo) const
	{
		if(!rtti_is_of_type<ScriptSerializableTypeInfoArray>(typeInfo))
			return false;

		auto arrayTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoArray>(typeInfo);

		return arrayTypeInfo->mRank == mRank && arrayTypeInfo->mElementType->matches(mElementType);
	}

	bool ScriptSerializableTypeInfoArray::isTypeLoaded() const
	{
		return mElementType->isTypeLoaded();
	}

	::MonoClass* ScriptSerializableTypeInfoArray::getMonoClass() const
	{
		::MonoClass* elementClass = mElementType->getMonoClass();
		if(elementClass == nullptr)
			return nullptr;

		return mono_array_class_get(mElementType->getMonoClass(), mRank);
	}

	RTTITypeBase* ScriptSerializableTypeInfoArray::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoArrayRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoArray::getRTTI() const
	{
		return ScriptSerializableTypeInfoArray::getRTTIStatic();
	}

	bool ScriptSerializableTypeInfoList::matches(const ScriptSerializableTypeInfoPtr& typeInfo) const
	{
		if(!rtti_is_of_type<ScriptSerializableTypeInfoList>(typeInfo))
			return false;

		auto listTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoList>(typeInfo);

		return listTypeInfo->mElementType->matches(mElementType);
	}

	bool ScriptSerializableTypeInfoList::isTypeLoaded() const
	{
		return mElementType->isTypeLoaded();
	}

	::MonoClass* ScriptSerializableTypeInfoList::getMonoClass() const
	{
		::MonoClass* elementClass = mElementType->getMonoClass();
		if(elementClass == nullptr)
			return nullptr;

		MonoClass* genericListClass = RuntimeScriptObjects::instance().getSystemGenericListClass();
		MonoType* genParams[1] = { mono_class_get_type(elementClass) };

		return mono_class_bind_generic_parameters(genericListClass->_getInternalClass(), 1, genParams, false);
	}

	RTTITypeBase* ScriptSerializableTypeInfoList::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoListRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoList::getRTTI() const
	{
		return ScriptSerializableTypeInfoList::getRTTIStatic();
	}

	bool ScriptSerializableTypeInfoDictionary::matches(const ScriptSerializableTypeInfoPtr& typeInfo) const
	{
		if(!rtti_is_of_type<ScriptSerializableTypeInfoDictionary>(typeInfo))
			return false;

		auto dictTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoDictionary>(typeInfo);

		return dictTypeInfo->mKeyType->matches(mKeyType) && dictTypeInfo->mValueType->matches(mValueType);
	}

	bool ScriptSerializableTypeInfoDictionary::isTypeLoaded() const
	{
		return mKeyType->isTypeLoaded() && mValueType->isTypeLoaded();
	}

	::MonoClass* ScriptSerializableTypeInfoDictionary::getMonoClass() const
	{
		::MonoClass* keyClass = mKeyType->getMonoClass();
		::MonoClass* valueClass = mValueType->getMonoClass();
		if(keyClass == nullptr || valueClass == nullptr)
			return nullptr;

		MonoClass* genericDictionaryClass = RuntimeScriptObjects::instance().getSystemGenericDictionaryClass();
		MonoType* genParams[2] = { mono_class_get_type(keyClass), mono_class_get_type(valueClass) };

		return mono_class_bind_generic_parameters(genericDictionaryClass->_getInternalClass(), 2, genParams, false);
	}

	RTTITypeBase* ScriptSerializableTypeInfoDictionary::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoDictionaryRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoDictionary::getRTTI() const
	{
		return ScriptSerializableTypeInfoDictionary::getRTTIStatic();
	}
}