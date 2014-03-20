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

	RTTITypeBase* ScriptSerializableTypeInfoPrimitive::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoPrimitiveRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoPrimitive::getRTTI() const
	{
		return ScriptSerializableTypeInfoPrimitive::getRTTIStatic();
	}

	RTTITypeBase* ScriptSerializableTypeInfoObject::getRTTIStatic()
	{
		return ScriptSerializableTypeInfoObjectRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableTypeInfoObject::getRTTI() const
	{
		return ScriptSerializableTypeInfoObject::getRTTIStatic();
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