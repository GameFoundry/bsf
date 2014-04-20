#include "BsScriptSerializableDictionary.h"
#include "BsScriptSerializableDictionaryRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"

namespace BansheeEngine
{
	ScriptSerializableDictionary::Enumerator::Enumerator(MonoObject* instance, const ScriptSerializableDictionary* parent)
		:mInstance(instance), mParent(parent), mCurrent(nullptr)
	{ }

	ScriptSerializableFieldDataPtr ScriptSerializableDictionary::Enumerator::getKey() const
	{
		MonoObject* obj = mParent->mKeyProp->get(mCurrent);

		return ScriptSerializableFieldData::create(mParent->mDictionaryTypeInfo->mKeyType, obj);
	}

	ScriptSerializableFieldDataPtr ScriptSerializableDictionary::Enumerator::getValue() const
	{
		MonoObject* obj = mParent->mValueProp->get(mCurrent);

		return ScriptSerializableFieldData::create(mParent->mDictionaryTypeInfo->mValueType, obj);
	}

	bool ScriptSerializableDictionary::Enumerator::moveNext()
	{
		MonoObject* returnVal = mParent->mEnumMoveNext->invoke(mInstance, nullptr);
		bool isValid = *(bool*)mono_object_unbox(returnVal);

		if(isValid)
			mCurrent = (MonoObject*)mono_object_unbox(mParent->mEnumCurrentProp->get(mInstance));
		else
			mCurrent = nullptr;

		return isValid;
	}

	ScriptSerializableDictionary::ScriptSerializableDictionary(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr)
	{ }

	ScriptSerializableDictionary::ScriptSerializableDictionary(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance)
		:mDictionaryTypeInfo(typeInfo), mManagedInstance(managedInstance), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr)
	{

	}

	ScriptSerializableDictionaryPtr ScriptSerializableDictionary::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		if(managedInstance == nullptr)
			return nullptr;

		::MonoClass* monoClass = mono_object_get_class(managedInstance);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);
		String fullName = elementNs + "." + elementTypeName;

		if(RuntimeScriptObjects::instance().getSystemGenericDictionaryClass()->getFullName() != fullName)
			return nullptr;

		return cm_shared_ptr<ScriptSerializableDictionary>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ScriptSerializableDictionaryPtr ScriptSerializableDictionary::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableDictionary>(ConstructPrivately());
	}

	void ScriptSerializableDictionary::serializeManagedInstance()
	{
		MonoClass* dictionaryClass = MonoManager::instance().findClass(mono_object_get_class(mManagedInstance));
		if(dictionaryClass == nullptr)
			return;

		initMonoObjects(dictionaryClass);

		mKeyEntries.clear();
		mValueEntries.clear();

		Enumerator enumerator = getEnumerator();

		while(enumerator.moveNext())
		{
			mKeyEntries.push_back(enumerator.getKey());
			mValueEntries.push_back(enumerator.getValue());
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

		initMonoObjects(dictionaryClass);

		mManagedInstance = dictionaryClass->createInstance();
		assert(mKeyEntries.size() == mValueEntries.size());

		for(UINT32 i = 0; i < (UINT32)mKeyEntries.size(); i++)
		{
			setFieldData(mKeyEntries[i], mValueEntries[i]);
		}
	}

	void ScriptSerializableDictionary::setFieldData(const ScriptSerializableFieldDataPtr& key, const ScriptSerializableFieldDataPtr& val)
	{
		void* params[2];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);
		params[1] = val->getValue(mDictionaryTypeInfo->mValueType);

		mAddMethod->invoke(mManagedInstance, params);
	}

	ScriptSerializableDictionary::Enumerator ScriptSerializableDictionary::getEnumerator() const
	{
		return Enumerator((MonoObject*)mono_object_unbox(mGetEnumerator->invoke(mManagedInstance, nullptr)), this);
	}

	void ScriptSerializableDictionary::initMonoObjects(MonoClass* dictionaryClass)
	{
		mAddMethod = &dictionaryClass->getMethod("Add", 2);
		mGetEnumerator = &dictionaryClass->getMethod("GetEnumerator");

		MonoClass* enumeratorClass = mGetEnumerator->getReturnType();
		assert(enumeratorClass != nullptr);

		mEnumMoveNext = &enumeratorClass->getMethod("MoveNext");
		mEnumCurrentProp = &enumeratorClass->getProperty("Current");

		MonoClass* keyValuePairClass = mEnumCurrentProp->getReturnType();
		assert(keyValuePairClass != nullptr);

		mKeyProp = &keyValuePairClass->getProperty("Key");
		mValueProp = &keyValuePairClass->getProperty("Value");
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