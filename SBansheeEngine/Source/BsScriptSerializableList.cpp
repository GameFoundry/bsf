#include "BsScriptSerializableList.h"
#include "BsScriptSerializableListRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableList::ScriptSerializableList(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mNumElements(0)
	{

	}

	ScriptSerializableList::ScriptSerializableList(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoListPtr& typeInfo, MonoObject* managedInstance)
		:mListTypeInfo(typeInfo), mManagedInstance(managedInstance), mNumElements(0)
	{
		mNumElements = getLength();
	}

	ScriptSerializableListPtr ScriptSerializableList::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoListPtr& typeInfo)
	{
		if(!RuntimeScriptObjects::instance().getSystemGenericListClass()->isInstanceOfType(managedInstance))
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

		mManagedInstance = listClass->createInstance();
		setLength(mNumElements);

		CM::UINT32 idx = 0;
		for(auto& arrayEntry : mListEntries)
		{
			setFieldData(idx, arrayEntry);
			idx++;
		}
	}

	void ScriptSerializableList::setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		if(val->isValueType())
			setValue(arrayIdx, val->getValue(mListTypeInfo->mElementType));
		else
		{
			MonoObject* ptrToObj = (MonoObject*)val->getValue(mListTypeInfo->mElementType);
			setValue(arrayIdx, &ptrToObj);
		}
	}

	ScriptSerializableFieldDataPtr ScriptSerializableList::getFieldData(CM::UINT32 arrayIdx)
	{
		return ScriptSerializableFieldData::create(mListTypeInfo->mElementType, getValue(arrayIdx));
	}

	void ScriptSerializableList::setValue(CM::UINT32 arrayIdx, void* val)
	{
		// TODO
	}

	void* ScriptSerializableList::getValue(CM::UINT32 arrayIdx)
	{
		// TODO
		
		return nullptr;
	}

	UINT32 ScriptSerializableList::getLength() const
	{
		// TODO
		
		return 0;
	}

	void ScriptSerializableList::setLength(UINT32 length)
	{
		// TODO
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