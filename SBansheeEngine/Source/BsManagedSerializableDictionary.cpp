#include "BsManagedSerializableDictionary.h"
#include "BsManagedSerializableDictionaryRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsManagedSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"

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
		:mManagedInstance(nullptr), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr)
	{ }

	ManagedSerializableDictionary::ManagedSerializableDictionary(const ConstructPrivately& dummy, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* managedInstance)
		:mDictionaryTypeInfo(typeInfo), mManagedInstance(managedInstance), mAddMethod(nullptr), mGetEnumerator(nullptr), mEnumMoveNext(nullptr),
		mEnumCurrentProp(nullptr), mKeyProp(nullptr), mValueProp(nullptr)
	{

	}

	ManagedSerializableDictionaryPtr ManagedSerializableDictionary::create(MonoObject* managedInstance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo)
	{
		if(managedInstance == nullptr)
			return nullptr;

		::MonoClass* monoClass = mono_object_get_class(managedInstance);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);
		String fullName = elementNs + "." + elementTypeName;

		if(RuntimeScriptObjects::instance().getSystemGenericDictionaryClass()->getFullName() != fullName)
			return nullptr;

		return bs_shared_ptr<ManagedSerializableDictionary>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ManagedSerializableDictionaryPtr ManagedSerializableDictionary::createEmpty()
	{
		return bs_shared_ptr<ManagedSerializableDictionary>(ConstructPrivately());
	}

	void ManagedSerializableDictionary::serializeManagedInstance()
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

	void ManagedSerializableDictionary::deserializeManagedInstance()
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

	void ManagedSerializableDictionary::setFieldData(const ManagedSerializableFieldDataPtr& key, const ManagedSerializableFieldDataPtr& val)
	{
		void* params[2];
		params[0] = key->getValue(mDictionaryTypeInfo->mKeyType);
		params[1] = val->getValue(mDictionaryTypeInfo->mValueType);

		mAddMethod->invoke(mManagedInstance, params);
	}

	ManagedSerializableDictionary::Enumerator ManagedSerializableDictionary::getEnumerator() const
	{
		return Enumerator((MonoObject*)mono_object_unbox(mGetEnumerator->invoke(mManagedInstance, nullptr)), this);
	}

	void ManagedSerializableDictionary::initMonoObjects(MonoClass* dictionaryClass)
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

	RTTITypeBase* ManagedSerializableDictionary::getRTTIStatic()
	{
		return ManagedSerializableDictionaryRTTI::instance();
	}

	RTTITypeBase* ManagedSerializableDictionary::getRTTI() const
	{
		return ManagedSerializableDictionary::getRTTIStatic();
	}
}