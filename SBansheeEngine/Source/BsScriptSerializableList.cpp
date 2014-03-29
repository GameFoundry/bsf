#include "BsScriptSerializableList.h"
#include "BsScriptSerializableListRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoProperty.h"

using namespace CamelotFramework;

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

	void ScriptSerializableList::setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		mItemProp->setIndexed(mManagedInstance, &arrayIdx, val->getValue(mListTypeInfo->mElementType));
	}


	void ScriptSerializableList::addFieldData(const ScriptSerializableFieldDataPtr& val)
	{
		bool isBoxedValueType = false;
		if(rtti_is_of_type<ScriptSerializableTypeInfoObject>(mListTypeInfo->mElementType))
		{
			ScriptSerializableTypeInfoObjectPtr objTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoObject>(mListTypeInfo->mElementType);
			isBoxedValueType = objTypeInfo->mValueType;
		}

		void* params[1];
		if(isBoxedValueType)
		{
			MonoObject* value = (MonoObject*)val->getValue(mListTypeInfo->mElementType);

			if(value != nullptr)
			{
				params[0] = mono_object_unbox(value); // Value types need to be set as native unboxed types
				mAddMethod->invoke(mManagedInstance, params);
			}
		}
		else
		{
			params[0] = val->getValue(mListTypeInfo->mElementType);
			mAddMethod->invoke(mManagedInstance, params);
		}
	}

	ScriptSerializableFieldDataPtr ScriptSerializableList::getFieldData(CM::UINT32 arrayIdx)
	{
		MonoObject* obj = mItemProp->getIndexed(mManagedInstance, &arrayIdx);

		if(mListTypeInfo->mElementType->isRawType())
		{
			void* unboxedValue = nullptr;
			if(obj != nullptr)
				unboxedValue = mono_object_unbox(obj);

			return ScriptSerializableFieldData::create(mListTypeInfo->mElementType, unboxedValue);
		}
		else
		{
			return ScriptSerializableFieldData::create(mListTypeInfo->mElementType, &obj);
		}	
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