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

		return objTypeInfo->mTypeNamespace == mTypeNamespace && objTypeInfo->mTypeName == mTypeName;
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

	RTTITypeBase* ScriptSerializableTypeInfoArray::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoArrayRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoArray::getRTTI() const
	{
		return ScriptSerializableTypeInfoArray::getRTTIStatic();
	}
}