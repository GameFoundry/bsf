#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectRTTI.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableField.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoField.h"

namespace BansheeEngine
{
	ManagedSerializableObject::ManagedSerializableObject(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr)
	{

	}

	ManagedSerializableObject::ManagedSerializableObject(const ConstructPrivately& dummy, ManagedSerializableObjectInfoPtr objInfo, MonoObject* managedInstance)
		:mObjInfo(objInfo), mManagedInstance(managedInstance)
	{

	}

	ManagedSerializableObjectPtr ManagedSerializableObject::createFromExisting(MonoObject* managedInstance)
	{
		if(managedInstance == nullptr)
			return nullptr;

		::MonoClass* monoClass = mono_object_get_class(managedInstance);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);

		ManagedSerializableObjectInfoPtr objInfo;
		if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(elementNs, elementTypeName, objInfo))
			return nullptr;

		return bs_shared_ptr<ManagedSerializableObject>(ConstructPrivately(), objInfo, managedInstance);
	}

	ManagedSerializableObjectPtr ManagedSerializableObject::createFromNew(const ManagedSerializableTypeInfoObjectPtr& type)
	{
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!RuntimeScriptObjects::instance().getSerializableObjectInfo(type->mTypeNamespace, type->mTypeName, currentObjInfo))
			return nullptr;

		return bs_shared_ptr<ManagedSerializableObject>(ConstructPrivately(), currentObjInfo, createManagedInstance(type));
	}

	MonoObject* ManagedSerializableObject::createManagedInstance(const ManagedSerializableTypeInfoObjectPtr& type)
	{
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!RuntimeScriptObjects::instance().getSerializableObjectInfo(type->mTypeNamespace, type->mTypeName, currentObjInfo))
			return nullptr;

		if (type->mValueType)
			return currentObjInfo->mMonoClass->createInstance(false);
		else
			return currentObjInfo->mMonoClass->createInstance();
	}

	ManagedSerializableObjectPtr ManagedSerializableObject::createEmpty()
	{
		return bs_shared_ptr<ManagedSerializableObject>(ConstructPrivately());
	}

	void ManagedSerializableObject::serializeManagedInstance()
	{
		ManagedSerializableObjectInfoPtr curType = mObjInfo;

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

				ManagedSerializableFieldKeyPtr fieldKey = ManagedSerializableFieldKey::create(curType->mTypeId, field.second->mFieldId);
				ManagedSerializableFieldDataPtr fieldData = getFieldData(field.second);

				mFieldEntries[curIdx] = ManagedSerializableFieldDataEntry::create(fieldKey, fieldData);
				curIdx++;
			}

			curType = curType->mBaseClass;
		}
	}

	void ManagedSerializableObject::deserializeManagedInstance()
	{
		ManagedSerializableObjectInfoPtr storedObjInfo = mObjInfo;
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if(!RuntimeScriptObjects::instance().getSerializableObjectInfo(storedObjInfo->mTypeInfo->mTypeNamespace, storedObjInfo->mTypeInfo->mTypeName, currentObjInfo))
			return;

		mManagedInstance = createManagedInstance(storedObjInfo->mTypeInfo);

		if (mManagedInstance == nullptr)
			return;

		auto findFieldInfoFromKey = [&] (UINT16 typeId, UINT16 fieldId, ManagedSerializableObjectInfoPtr objInfo, 
			ManagedSerializableFieldInfoPtr& outFieldInfo, ManagedSerializableObjectInfoPtr &outObjInfo) -> bool
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

		auto findTypeNameMatchingFieldInfo = [&] (const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableObjectInfoPtr& fieldObjInfo, 
			ManagedSerializableObjectInfoPtr objInfo) -> ManagedSerializableFieldInfoPtr
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
							ManagedSerializableFieldInfoPtr foundField = iterFind2->second;
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
			ManagedSerializableFieldInfoPtr storedFieldEntry;
			ManagedSerializableObjectInfoPtr storedFieldObjEntry;

			if(!findFieldInfoFromKey(fieldEntry->mKey->mTypeId, fieldEntry->mKey->mFieldId, storedObjInfo, storedFieldEntry, storedFieldObjEntry))
				continue;

			ManagedSerializableFieldInfoPtr matchingFieldInfo = findTypeNameMatchingFieldInfo(storedFieldEntry, storedFieldObjEntry, currentObjInfo);
			if(matchingFieldInfo != nullptr)
				setFieldData(matchingFieldInfo, fieldEntry->mValue);
		}
	}

	void ManagedSerializableObject::setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val)
	{
		fieldInfo->mMonoField->setValue(mManagedInstance, val->getValue(fieldInfo->mTypeInfo));
	}

	ManagedSerializableFieldDataPtr ManagedSerializableObject::getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo)
	{
		MonoObject* fieldValue = fieldInfo->mMonoField->getValueBoxed(mManagedInstance);

		return ManagedSerializableFieldData::create(fieldInfo->mTypeInfo, fieldValue);		
	}

	RTTITypeBase* ManagedSerializableObject::getRTTIStatic()
	{
		return ManagedSerializableObjectRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableObject::getRTTI() const
	{
		return ManagedSerializableObject::getRTTIStatic();
	}
}