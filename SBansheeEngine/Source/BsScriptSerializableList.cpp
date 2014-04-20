#include "BsScriptSerializableList.h"
#include "BsScriptSerializableListRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"

namespace BansheeEngine
{
	ScriptSerializableList::ScriptSerializableList(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mNumElements(0), mItemProp(nullptr), mCountProp(nullptr), mAddMethod(nullptr)
	{

	}

	ScriptSerializableList::ScriptSerializableList(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoListPtr& typeInfo, MonoObject* managedInstance)
		:mListTypeInfo(typeInfo), mManagedInstance(managedInstance), mNumElements(0), mItemProp(nullptr),
		mCountProp(nullptr), mAddMethod(nullptr)
	{
		MonoClass* listClass = MonoManager::instance().findClass(mono_object_get_class(managedInstance));
		if(listClass == nullptr)
			return;

		initMonoObjects(listClass);

		mNumElements = getLength();
	}

	ScriptSerializableListPtr ScriptSerializableList::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoListPtr& typeInfo)
	{
		if(managedInstance == nullptr)
			return nullptr;

		::MonoClass* monoClass = mono_object_get_class(managedInstance);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);
		String fullName = elementNs + "." + elementTypeName;

		if(RuntimeScriptObjects::instance().getSystemGenericListClass()->getFullName() != fullName)
			return nullptr;

		return cm_shared_ptr<ScriptSerializableList>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ScriptSerializableListPtr ScriptSerializableList::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableList>(ConstructPrivately());
	}

	void ScriptSerializableList::serializeManagedInstance()
	{
		mListEntries.resize(mNumElements);
		for(UINT32 i = 0; i < mNumElements; i++)
		{
			mListEntries[i] = getFieldData(i);
		}
	}

	void ScriptSerializableList::deserializeManagedInstance()
	{
		if(!mListTypeInfo->isTypeLoaded())
			return;

		::MonoClass* listMonoClass = mListTypeInfo->getMonoClass();
		MonoClass* listClass = MonoManager::instance().findClass(listMonoClass);
		if(listClass == nullptr)
			return;

		initMonoObjects(listClass);

		void* params[1] = { &mNumElements };
		mManagedInstance = listClass->createInstance("int", params);

		for(auto& arrayEntry : mListEntries)
		{
			addFieldData(arrayEntry);
		}
	}

	void ScriptSerializableList::setFieldData(UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		mItemProp->setIndexed(mManagedInstance, &arrayIdx, val->getValue(mListTypeInfo->mElementType));
	}


	void ScriptSerializableList::addFieldData(const ScriptSerializableFieldDataPtr& val)
	{
		void* params[1];
		params[0] = val->getValue(mListTypeInfo->mElementType);
		mAddMethod->invoke(mManagedInstance, params);
	}

	ScriptSerializableFieldDataPtr ScriptSerializableList::getFieldData(UINT32 arrayIdx)
	{
		MonoObject* obj = mItemProp->getIndexed(mManagedInstance, &arrayIdx);

		return ScriptSerializableFieldData::create(mListTypeInfo->mElementType, obj);
	}

	UINT32 ScriptSerializableList::getLength() const
	{
		MonoObject* length = mCountProp->get(mManagedInstance);

		if(length == nullptr)
			return 0;

		return *(UINT32*)mono_object_unbox(length);
	}

	void ScriptSerializableList::initMonoObjects(MonoClass* listClass)
	{
		mItemProp = &listClass->getProperty("Item");
		mCountProp = &listClass->getProperty("Count");
		mAddMethod = &listClass->getMethod("Add", 1);
	}

	RTTITypeBase* ScriptSerializableList::getRTTIStatic()
	{
		return ScriptSerializableListRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableList::getRTTI() const
	{
		return ScriptSerializableList::getRTTIStatic();
	}
}