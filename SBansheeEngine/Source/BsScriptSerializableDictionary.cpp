#include "BsScriptSerializableDictionary.h"
#include "BsScriptSerializableDictionaryRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableDictionary::ScriptSerializableDictionary(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mNumElements(0)
	{

	}

	ScriptSerializableDictionary::ScriptSerializableDictionary(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance)
		:mDictionaryTypeInfo(typeInfo), mManagedInstance(managedInstance), mNumElements(0)
	{
		mNumElements = getLength();
	}

	ScriptSerializableDictionaryPtr ScriptSerializableDictionary::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		if(!RuntimeScriptObjects::instance().getSystemGenericDictionaryClass()->isInstanceOfType(managedInstance))
			return nullptr;

		return cm_shared_ptr<ScriptSerializableDictionary>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ScriptSerializableDictionaryPtr ScriptSerializableDictionary::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableDictionary>(ConstructPrivately());
	}

	void ScriptSerializableDictionary::serializeManagedInstance()
	{
		mDictionaryEntries.resize(mNumElements * 2);
		for(UINT32 i = 0; i < mNumElements; i++)
		{
			mDictionaryEntries[i] = getFieldData(i);
		}
	}

	void ScriptSerializableDictionary::deserializeManagedInstance()
	{
		if(!mDictionaryTypeInfo->isTypeLoaded())
			return;

		::MonoClass* dictionaryMonoClass = mDictionaryTypeInfo->getMonoClass();
		MonoClass* dictionaryClass = MonoManager::instance().findClass(dictionaryMonoClass);
		if(dictionaryClass == nullptr)
			return;

		mManagedInstance = dictionaryClass->createInstance();
		setLength(mNumElements);

		CM::UINT32 idx = 0;
		for(auto& arrayEntry : mDictionaryEntries)
		{
			setFieldData(idx, arrayEntry);
			idx++;
		}
	}

	void ScriptSerializableDictionary::setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		if(val->isValueType())
		{
			if((arrayIdx % 2) == 0)
				setValue(arrayIdx, val->getValue(mDictionaryTypeInfo->mKeyType));
			else
				setValue(arrayIdx, val->getValue(mDictionaryTypeInfo->mValueType));
		}
		else
		{
			MonoObject* ptrToObj = nullptr;
			
			if((arrayIdx % 2) == 0)
				ptrToObj = (MonoObject*)val->getValue(mDictionaryTypeInfo->mKeyType);
			else
				ptrToObj = (MonoObject*)val->getValue(mDictionaryTypeInfo->mValueType);

			setValue(arrayIdx, &ptrToObj);
		}
	}

	ScriptSerializableFieldDataPtr ScriptSerializableDictionary::getFieldData(CM::UINT32 arrayIdx)
	{
		if((arrayIdx % 2) == 0)
			return ScriptSerializableFieldData::create(mDictionaryTypeInfo->mKeyType, getValue(arrayIdx));
		else
			return ScriptSerializableFieldData::create(mDictionaryTypeInfo->mValueType, getValue(arrayIdx));
	}

	void ScriptSerializableDictionary::setValue(CM::UINT32 arrayIdx, void* val)
	{
		// TODO
	}

	void* ScriptSerializableDictionary::getValue(CM::UINT32 arrayIdx)
	{
		// TODO

		return nullptr;
	}

	UINT32 ScriptSerializableDictionary::getLength() const
	{
		// TODO

		return 0;
	}

	void ScriptSerializableDictionary::setLength(UINT32 length)
	{
		// TODO
	}

	RTTITypeBase* ScriptSerializableDictionary::getRTTIStatic()
	{
		return ScriptSerializableDictionaryRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableDictionary::getRTTI() const
	{
		return ScriptSerializableDictionary::getRTTIStatic();
	}
}