#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectRTTI.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsManagedSerializableField.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	inline size_t ManagedSerializableObject::Hash::operator()(const ManagedSerializableFieldKey& x) const
	{
		size_t seed = 0;
		hash_combine(seed, (UINT32)x.mFieldId);
		hash_combine(seed, (UINT32)x.mTypeId);

		return seed;
	}

	inline bool ManagedSerializableObject::Equals::operator()(const ManagedSerializableFieldKey& a, const ManagedSerializableFieldKey& b) const
	{
		return a.mFieldId == b.mFieldId && a.mTypeId == b.mTypeId;
	}

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

	ManagedSerializableObjectPtr ManagedSerializableObject::createNew(const ManagedSerializableTypeInfoObjectPtr& type)
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

	void ManagedSerializableObject::serialize()
	{
		if (mManagedInstance == nullptr)
			return;

		mCachedData.clear();

		ManagedSerializableObjectInfoPtr curType = mObjInfo;
		while (curType != nullptr)
		{
			for (auto& field : curType->mFields)
			{
				if (field.second->isSerializable())
				{
					ManagedSerializableFieldKey key(field.second->mParentTypeId, field.second->mFieldId);
					mCachedData[key] = getFieldData(field.second);
				}
			}

			curType = curType->mBaseClass;
		}

		// Serialize children
		for (auto& fieldEntry : mCachedData)
			fieldEntry.second->serialize();

		mManagedInstance = nullptr;
	}

	void ManagedSerializableObject::deserialize()
	{
		mManagedInstance = createManagedInstance(mObjInfo->mTypeInfo);

		if (mManagedInstance == nullptr)
		{
			mCachedData.clear();
			return;
		}

		ManagedSerializableObjectInfoPtr currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(mObjInfo->mTypeInfo->mTypeNamespace, mObjInfo->mTypeInfo->mTypeName, currentObjInfo))
		{
			mCachedData.clear();
			return;
		}

		// Deserialize children
		for (auto& fieldEntry : mCachedData)
			fieldEntry.second->deserialize();

		// Scan all fields and ensure the fields still exist
		UINT32 i = 0;
		ManagedSerializableObjectInfoPtr curType = mObjInfo;
		while (curType != nullptr)
		{
			for (auto& field : curType->mFields)
			{
				if (field.second->isSerializable())
				{
					UINT32 fieldId = field.second->mFieldId;
					UINT32 typeID = field.second->mParentTypeId;

					ManagedSerializableFieldKey key(typeID, fieldId);

					ManagedSerializableFieldInfoPtr matchingFieldInfo = currentObjInfo->findMatchingField(field.second, curType->mTypeInfo);
					if (matchingFieldInfo != nullptr)
						setFieldData(matchingFieldInfo, mCachedData[key]);

					i++;
				}
			}

			curType = curType->mBaseClass;
		}

		mObjInfo = currentObjInfo;
		mCachedData.clear();
	}

	void ManagedSerializableObject::setFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo, const ManagedSerializableFieldDataPtr& val)
	{
		if (mManagedInstance != nullptr)
			fieldInfo->mMonoField->setValue(mManagedInstance, val->getValue(fieldInfo->mTypeInfo));
		else
		{
			ManagedSerializableFieldKey key(fieldInfo->mParentTypeId, fieldInfo->mFieldId);
			mCachedData[key] = val;
		}
	}

	ManagedSerializableFieldDataPtr ManagedSerializableObject::getFieldData(const ManagedSerializableFieldInfoPtr& fieldInfo) const
	{
		if (mManagedInstance != nullptr)
		{
			MonoObject* fieldValue = fieldInfo->mMonoField->getValueBoxed(mManagedInstance);

			return ManagedSerializableFieldData::create(fieldInfo->mTypeInfo, fieldValue);
		}
		else
		{
			ManagedSerializableFieldKey key(fieldInfo->mParentTypeId, fieldInfo->mFieldId);
			auto iterFind = mCachedData.find(key);

			if (iterFind != mCachedData.end())
				return iterFind->second;

			return nullptr;
		}
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