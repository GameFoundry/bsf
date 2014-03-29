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

	}

	ScriptSerializableObjectPtr ScriptSerializableObject::create(MonoObject* managedInstance)
	{
		if(managedInstance == nullptr)
			return nullptr;

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

	void ScriptSerializableObject::serializeManagedInstance()
	{
		ScriptSerializableObjectInfoPtr curType = mObjInfo;

		UINT32 numFields = 0;
		while(curType != nullptr)
		{
			for(auto& field : mObjInfo->mFields)
			{
				if(field.second->isSerializable())
					numFields++;
			}
			curType = curType->mBaseClass;
		}

		mFieldEntries.resize(numFields);
		curType = mObjInfo;
		UINT32 curIdx = 0;
		while(curType != nullptr)
		{
			for(auto& field : mObjInfo->mFields)
			{
				if(!field.second->isSerializable())
					continue;

				ScriptSerializableFieldKeyPtr fieldKey = ScriptSerializableFieldKey::create(curType->mTypeId, field.second->mFieldId);
				ScriptSerializableFieldDataPtr fieldData = getFieldData(field.second);

				mFieldEntries[curIdx] = ScriptSerializableFieldDataEntry::create(fieldKey, fieldData);
				curIdx++;
			}

			curType = curType->mBaseClass;
		}
	}

	void ScriptSerializableObject::deserializeManagedInstance()
	{
		ScriptSerializableObjectInfoPtr storedObjInfo = mObjInfo;
		ScriptSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(storedObjInfo->mTypeInfo->mTypeNamespace, storedObjInfo->mTypeInfo->mTypeName, currentObjInfo))
			return;

		if(storedObjInfo->mTypeInfo->mValueType)
			mManagedInstance = currentObjInfo->mMonoClass->createInstance(false);
		else
			mManagedInstance = currentObjInfo->mMonoClass->createInstance();

		auto findFieldInfoFromKey = [&] (CM::UINT16 typeId, CM::UINT16 fieldId, ScriptSerializableObjectInfoPtr objInfo, 
			ScriptSerializableFieldInfoPtr& outFieldInfo, ScriptSerializableObjectInfoPtr &outObjInfo) -> bool
		{
			while(objInfo != nullptr)
			{
				if(objInfo->mTypeId == typeId)
				{
					auto iterFind = objInfo->mFields.find(fieldId);
					if(iterFind != objInfo->mFields.end())
					{
						outFieldInfo = iterFind->second;
						outObjInfo = objInfo;

						return true;
					}

					return false;
				}

				objInfo = objInfo->mBaseClass;
			}

			return false;
		};

		auto findTypeNameMatchingFieldInfo = [&] (const ScriptSerializableFieldInfoPtr& fieldInfo, const ScriptSerializableObjectInfoPtr& fieldObjInfo, 
			ScriptSerializableObjectInfoPtr objInfo) -> ScriptSerializableFieldInfoPtr
		{
			while(objInfo != nullptr)
			{
				if(objInfo->mTypeInfo->matches(fieldObjInfo->mTypeInfo))
				{
					auto iterFind = objInfo->mFieldNameToId.find(fieldInfo->mName);
					if(iterFind != objInfo->mFieldNameToId.end())
					{
						auto iterFind2 = objInfo->mFields.find(iterFind->second);
						if(iterFind2 != objInfo->mFields.end())
						{
							ScriptSerializableFieldInfoPtr foundField = iterFind2->second;
							if(foundField->isSerializable())
							{
								if(fieldInfo->mTypeInfo->matches(foundField->mTypeInfo))
									return foundField;
							}
						}
					}

					return nullptr;
				}

				objInfo = objInfo->mBaseClass;
			}

			return nullptr;
		};

		// Scan all fields and ensure the fields still exist
		for(auto& fieldEntry : mFieldEntries)
		{
			ScriptSerializableFieldInfoPtr storedFieldEntry;
			ScriptSerializableObjectInfoPtr storedFieldObjEntry;

			if(!findFieldInfoFromKey(fieldEntry->mKey->mTypeId, fieldEntry->mKey->mFieldId, storedObjInfo, storedFieldEntry, storedFieldObjEntry))
				continue;

			ScriptSerializableFieldInfoPtr matchingFieldInfo = findTypeNameMatchingFieldInfo(storedFieldEntry, storedFieldObjEntry, currentObjInfo);
			if(matchingFieldInfo != nullptr)
				setFieldData(matchingFieldInfo, fieldEntry->mValue);
		}
	}

	void ScriptSerializableObject::setFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo, const ScriptSerializableFieldDataPtr& val)
	{
		fieldInfo->mMonoField->setValue(mManagedInstance, val->getValue(fieldInfo->mTypeInfo));
	}

	ScriptSerializableFieldDataPtr ScriptSerializableObject::getFieldData(const ScriptSerializableFieldInfoPtr& fieldInfo)
	{
		MonoObject* fieldValue = fieldInfo->mMonoField->getValueBoxed(mManagedInstance);

		return ScriptSerializableFieldData::create(fieldInfo->mTypeInfo, fieldValue);		
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