#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableObjectRTTI.h"
#include "BsScriptSerializableObjectInfo.h"
#include "BsScriptSerializableField.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoField.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableObject::ScriptSerializableObject(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr)
	{

	}

	ScriptSerializableObject::ScriptSerializableObject(const ConstructPrivately& dummy, ScriptSerializableObjectInfoPtr objInfo, MonoObject* managedInstance)
		:mObjInfo(objInfo), mManagedInstance(managedInstance)
	{
		ScriptSerializableObjectInfoPtr curType = mObjInfo;

		UINT32 numFields = 0;
		while(curType != nullptr)
		{
			numFields += (UINT32)mObjInfo->mFields.size();
			curType = curType->mBaseClass.lock();
		}

		mFieldEntries.resize(numFields);
		curType = mObjInfo;
		UINT32 curIdx = 0;
		while(curType != nullptr)
		{
			numFields += (UINT32)mObjInfo->mFields.size();

			for(auto& field : mObjInfo->mFields)
			{
				ScriptSerializableFieldKeyPtr fieldKey = ScriptSerializableFieldKey::create(curType->mTypeId, field.second->mFieldId);
				ScriptSerializableFieldDataPtr fieldData = getFieldData(field.second);

				mFieldEntries[curIdx] = ScriptSerializableFieldDataEntry::create(fieldKey, fieldData);
				curIdx++;
			}

			curType = curType->mBaseClass.lock();
		}
	}

	ScriptSerializableObjectPtr ScriptSerializableObject::create(MonoObject* managedInstance)
	{
		::MonoClass* monoClass = mono_object_get_class(managedInstance);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);

		ScriptSerializableObjectInfoPtr objInfo;
		if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(elementNs, elementTypeName, objInfo))
			return nullptr;

		return cm_shared_ptr<ScriptSerializableObject>(ConstructPrivately(), objInfo, managedInstance);
	}

	ScriptSerializableObjectPtr ScriptSerializableObject::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableObject>(ConstructPrivately());
	}

	void ScriptSerializableObject::setFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo, const ScriptSerializableFieldDataPtr& val)
	{
		setValue(fieldInfo, val->getValue(fieldInfo->mTypeInfo));
	}

	ScriptSerializableFieldDataPtr ScriptSerializableObject::getFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo)
	{
		return ScriptSerializableFieldData::create(fieldInfo->mTypeInfo, getValue(fieldInfo));
	}

	void ScriptSerializableObject::setValue(const ScriptSerializableFieldInfoPtr& fieldInfo, void* val)
	{
		fieldInfo->mMonoField->setValue(mManagedInstance, val);
	}

	void* ScriptSerializableObject::getValue(const ScriptSerializableFieldInfoPtr& fieldInfo)
	{
		return fieldInfo->mMonoField->getValue(mManagedInstance);
	}

	RTTITypeBase* ScriptSerializableObject::getRTTIStatic()
	{
		return ScriptSerializableObjectRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableObject::getRTTI() const
	{
		return ScriptSerializableObject::getRTTIStatic();
	}
}