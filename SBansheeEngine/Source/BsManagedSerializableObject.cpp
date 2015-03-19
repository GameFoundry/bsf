#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectRTTI.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableField.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObjectData.h"
#include "BsMonoField.h"
#include "BsMonoUtil.h"

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

		String elementNs;
		String elementTypeName;
		MonoUtil::getClassName(managedInstance, elementNs, elementTypeName);

		ManagedSerializableObjectInfoPtr objInfo;
		if(!ScriptAssemblyManager::instance().getSerializableObjectInfo(elementNs, elementTypeName, objInfo))
			return nullptr;

		return bs_shared_ptr<ManagedSerializableObject>(ConstructPrivately(), objInfo, managedInstance);
	}

	ManagedSerializableObjectPtr ManagedSerializableObject::createFromNew(const ManagedSerializableTypeInfoObjectPtr& type)
	{
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(type->mTypeNamespace, type->mTypeName, currentObjInfo))
			return nullptr;

		return bs_shared_ptr<ManagedSerializableObject>(ConstructPrivately(), currentObjInfo, createManagedInstance(type));
	}

	MonoObject* ManagedSerializableObject::createManagedInstance(const ManagedSerializableTypeInfoObjectPtr& type)
	{
		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(type->mTypeNamespace, type->mTypeName, currentObjInfo))
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

	void ManagedSerializableObject::deserializeManagedInstance(const Vector<ManagedSerializableFieldDataEntryPtr>& entries)
	{
		mManagedInstance = createManagedInstance(mObjInfo->mTypeInfo);

		if (mManagedInstance == nullptr)
			return;

		setFieldEntries(entries, mObjInfo);
	}

	void ManagedSerializableObject::setFieldEntries(const Vector<ManagedSerializableFieldDataEntryPtr>& entries, const ManagedSerializableObjectInfoPtr& originalEntriesType)
	{
		auto findFieldInfoFromKey = [&](UINT16 typeId, UINT16 fieldId, ManagedSerializableObjectInfoPtr objInfo,
			ManagedSerializableFieldInfoPtr& outFieldInfo, ManagedSerializableObjectInfoPtr &outObjInfo) -> bool
		{
			while (objInfo != nullptr)
			{
				if (objInfo->mTypeId == typeId)
				{
					auto iterFind = objInfo->mFields.find(fieldId);
					if (iterFind != objInfo->mFields.end())
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

		auto findTypeNameMatchingFieldInfo = [&](const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableObjectInfoPtr& fieldObjInfo,
			ManagedSerializableObjectInfoPtr objInfo) -> ManagedSerializableFieldInfoPtr
		{
			while (objInfo != nullptr)
			{
				if (objInfo->mTypeInfo->matches(fieldObjInfo->mTypeInfo))
				{
					auto iterFind = objInfo->mFieldNameToId.find(fieldInfo->mName);
					if (iterFind != objInfo->mFieldNameToId.end())
					{
						auto iterFind2 = objInfo->mFields.find(iterFind->second);
						if (iterFind2 != objInfo->mFields.end())
						{
							ManagedSerializableFieldInfoPtr foundField = iterFind2->second;
							if (foundField->isSerializable())
							{
								if (fieldInfo->mTypeInfo->matches(foundField->mTypeInfo))
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

		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(originalEntriesType->mTypeInfo->mTypeNamespace, originalEntriesType->mTypeInfo->mTypeName, currentObjInfo))
			return;

		// Scan all fields and ensure the fields still exist
		for (auto& fieldEntry : entries)
		{
			ManagedSerializableFieldInfoPtr storedFieldEntry;
			ManagedSerializableObjectInfoPtr storedFieldObjEntry;

			if (!findFieldInfoFromKey(fieldEntry->mKey->mTypeId, fieldEntry->mKey->mFieldId, originalEntriesType, storedFieldEntry, storedFieldObjEntry))
				continue;

			ManagedSerializableFieldInfoPtr matchingFieldInfo = findTypeNameMatchingFieldInfo(storedFieldEntry, storedFieldObjEntry, currentObjInfo);
			if (matchingFieldInfo != nullptr)
				setFieldData(matchingFieldInfo, fieldEntry->mValue);
		}
	}

	void ManagedSerializableObject::setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val)
	{
		fieldInfo->mMonoField->setValue(mManagedInstance, val->getValue(fieldInfo->mTypeInfo));
	}

	ManagedSerializableFieldDataPtr ManagedSerializableObject::getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo) const
	{
		MonoObject* fieldValue = fieldInfo->mMonoField->getValueBoxed(mManagedInstance);

		return ManagedSerializableFieldData::create(fieldInfo->mTypeInfo, fieldValue);
	}

	void ManagedSerializableObject::setObjectData(const ManagedSerializableObjectDataPtr& objectData, const ManagedSerializableObjectInfoPtr& originalEntriesType)
	{
		setFieldEntries(objectData->mFieldData, originalEntriesType);
	}

	ManagedSerializableObjectDataPtr ManagedSerializableObject::getObjectData() const
	{
		ManagedSerializableObjectDataPtr objectData = bs_shared_ptr<ManagedSerializableObjectData>();
		objectData->mFieldData = Vector<ManagedSerializableFieldDataEntryPtr>(mObjInfo->mCachedAllFields.size());

		UINT32 i = 0;
		for (auto& field : mObjInfo->mCachedAllFields)
		{
			ManagedSerializableFieldKeyPtr fieldKey = ManagedSerializableFieldKey::create(field.parentTypeId, field.info->mFieldId);
			ManagedSerializableFieldDataPtr fieldData = getFieldData(field.info);

			objectData->mFieldData[i] = ManagedSerializableFieldDataEntry::create(fieldKey, fieldData);
			i++;
		}

		return objectData;
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