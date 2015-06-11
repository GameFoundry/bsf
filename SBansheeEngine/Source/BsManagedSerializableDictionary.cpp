#include "BsManagedSerializableDictionary.h"
#include "BsManagedSerializableDictionaryRTTI.h"
#include "BsMonoManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ManagedSerializableDictionary::Enumerator::Enumerator(MonoObject* instance, const ManagedSerializableDictionary* parent)
		:mInstance(instance), mParent(parent), mCurrent(nullptr)
	{ }

	ManagedSerializableFieldDataPtr ManagedSerializableDictionary::Enumerator::getKey() const
	{
		MonoObject* obj = mParent->mKeyProp->get(mCurrent);

		return ManagedSerializableFieldData::create(mParent->mDictionaryTypeInfo->mKeyType, obj);
	}

	ManagedSerializableFieldDataPtr ManagedSerializableDictionary::Enumerator::getValue() const
	{
		MonoObject* obj = mParent->mValueProp->get(mCurrent);

		return ManagedSerializableFieldData::create(mParent->mDictionaryTypeInfo->mValueType, obj);
	}

	bool ManagedSerializableDictionary::Enumerator::moveNext()
	{
		MonoObject* returnVal = mParent->mEnumMoveNext->invoke(mInstance, nullptr);
		bool isValid = *(bool*)mono_object_unbox(returnVal);

		if(isValid)
			mCurrent = (MonoObject*)mono_object_unbox(mParent->mEnumCurrentProp->get(mInstance));
		else
			mCurrent = nullptr;

		return isValid;
	}

	ManagedSerializableDictionary::ManagedSerializableDictionary(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr), mRemoveMethod(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr), mContainsKeyMethod(nullptr), mTryGetValueMethod(nullptr)
	{ }

	ManagedSerializableDictionary::ManagedSerializableDictionary(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance)
		:mDictionaryTypeInfo(typeInfo), mManagedInstance(managedInstance), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr), mContainsKeyMethod(nullptr), mTryGetValueMethod(nullptr), mRemoveMethod(nullptr)
	{
		MonoClass* dictClass = MonoManager::instance().findClass(mono_object_get_class(managedInstance));
		if (dictClass == nullptr)
			return;

		initMonoObjects(dictClass);
	}

	ManagedSerializableDictionaryPtr ManagedSerializableDictionary::createFromExisting(MonoObject* managedInstance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		if(managedInstance == nullptr)
			return nullptr;

		String elementNs;
		String elementTypeName;
		MonoUtil::getClassName(managedInstance, elementNs, elementTypeName);

		String fullName = elementNs + "." + elementTypeName;

		if(ScriptAssemblyManager::instance().getSystemGenericDictionaryClass()->getFullName() != fullName)
			return nullptr;

		return bs_shared_ptr<ManagedSerializableDictionary>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ManagedSerializableDictionaryPtr ManagedSerializableDictionary::createNew(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		return bs_shared_ptr<ManagedSerializableDictionary>(ConstructPrivately(), typeInfo, createManagedInstance(typeInfo));
	}

	MonoObject* ManagedSerializableDictionary::createManagedInstance(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		if (!typeInfo->isTypeLoaded())
			return nullptr;

		::MonoClass* dictionaryMonoClass = typeInfo->getMonoClass();
		MonoClass* dictionaryClass = MonoManager::instance().findClass(dictionaryMonoClass);
		if (dictionaryClass == nullptr)
			return nullptr;

		return dictionaryClass->createInstance();
	}

	ManagedSerializableDictionaryPtr ManagedSerializableDictionary::createEmpty()
	{
		return bs_shared_ptr<ManagedSerializableDictionary>(ConstructPrivately());
	}

	void ManagedSerializableDictionary::serializeManagedInstance(Vector<ManagedSerializableFieldDataPtr>& keyEntries,
		Vector<ManagedSerializableFieldDataPtr>& valueEntries)
	{
		MonoClass* dictionaryClass = MonoManager::instance().findClass(mono_object_get_class(mManagedInstance));
		if(dictionaryClass == nullptr)
			return;

		initMonoObjects(dictionaryClass);

		keyEntries.clear();
		valueEntries.clear();

		Enumerator enumerator = getEnumerator();

		while(enumerator.moveNext())
		{
			keyEntries.push_back(enumerator.getKey());
			valueEntries.push_back(enumerator.getValue());
		}
	}

	void ManagedSerializableDictionary::deserializeManagedInstance(const Vector<ManagedSerializableFieldDataPtr>& keyEntries,
		const Vector<ManagedSerializableFieldDataPtr>& valueEntries)
	{
		mManagedInstance = createManagedInstance(mDictionaryTypeInfo);
		if (mManagedInstance == nullptr)
			return;

		::MonoClass* dictionaryMonoClass = mDictionaryTypeInfo->getMonoClass();
		MonoClass* dictionaryClass = MonoManager::instance().findClass(dictionaryMonoClass);
		if (dictionaryClass == nullptr)
			return;

		initMonoObjects(dictionaryClass);

		assert(keyEntries.size() == valueEntries.size());

		for (UINT32 i = 0; i < (UINT32)keyEntries.size(); i++)
		{
			setFieldData(keyEntries[i], valueEntries[i]);
		}
	}

	ManagedSerializableFieldDataPtr ManagedSerializableDictionary::getFieldData(const ManagedSerializableFieldDataPtr& key)
	{
		MonoObject* value = nullptr;

		void* params[2];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);
		params[1] = &value;

		mTryGetValueMethod->invoke(mManagedInstance, params);

		MonoObject* boxedValue = value;
		::MonoClass* valueTypeClass = mDictionaryTypeInfo->mValueType->getMonoClass();
		if (mono_class_is_valuetype(valueTypeClass))
		{
			if (value != nullptr)
				boxedValue = mono_value_box(MonoManager::instance().getDomain(), valueTypeClass, &value);
		}

		return ManagedSerializableFieldData::create(mDictionaryTypeInfo->mValueType, boxedValue);
	}

	void ManagedSerializableDictionary::setFieldData(const ManagedSerializableFieldDataPtr& key, const ManagedSerializableFieldDataPtr& val)
	{
		void* params[2];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);
		params[1] = val->getValue(mDictionaryTypeInfo->mValueType);

		mAddMethod->invoke(mManagedInstance, params);
	}

	void ManagedSerializableDictionary::removeFieldData(const ManagedSerializableFieldDataPtr& key)
	{
		void* params[1];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);

		mRemoveMethod->invoke(mManagedInstance, params);
	}

	bool ManagedSerializableDictionary::contains(const ManagedSerializableFieldDataPtr& key) const
	{
		void* params[1];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);

		MonoObject* returnVal = mContainsKeyMethod->invoke(mManagedInstance, params);
		return *(bool*)mono_object_unbox(returnVal);
	}

	ManagedSerializableDictionary::Enumerator ManagedSerializableDictionary::getEnumerator() const
	{
		return Enumerator((MonoObject*)mono_object_unbox(mGetEnumerator->invoke(mManagedInstance, nullptr)), this);
	}

	void ManagedSerializableDictionary::initMonoObjects(MonoClass* dictionaryClass)
	{
		mAddMethod = dictionaryClass->getMethod("Add", 2);
		mRemoveMethod = dictionaryClass->getMethod("Remove", 1);
		mTryGetValueMethod = dictionaryClass->getMethod("TryGetValue", 2);
		mContainsKeyMethod = dictionaryClass->getMethod("ContainsKey", 1);
		mGetEnumerator = dictionaryClass->getMethod("GetEnumerator");

		MonoClass* enumeratorClass = mGetEnumerator->getReturnType();
		assert(enumeratorClass != nullptr);

		mEnumMoveNext = enumeratorClass->getMethod("MoveNext");
		mEnumCurrentProp = &enumeratorClass->getProperty("Current");

		MonoClass* keyValuePairClass = mEnumCurrentProp->getReturnType();
		assert(keyValuePairClass != nullptr);

		mKeyProp = &keyValuePairClass->getProperty("Key");
		mValueProp = &keyValuePairClass->getProperty("Value");
	}

	RTTITypeBase* ManagedSerializableDictionary::getRTTIStatic()
	{
		return ManagedSerializableDictionaryRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableDictionary::getRTTI() const
	{
		return ManagedSerializableDictionary::getRTTIStatic();
	}
}