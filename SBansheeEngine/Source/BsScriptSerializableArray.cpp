#include "BsScriptSerializableArray.h"
#include "BsScriptSerializableArrayRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableArray::ScriptSerializableArray(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mNumElements(0), mClass(nullptr)
	{

	}

	ScriptSerializableArray::ScriptSerializableArray(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoArrayPtr& typeInfo, MonoObject* managedInstance)
		:mArrayTypeInfo(typeInfo), mManagedInstance(managedInstance), mNumElements(0)
	{
		mClass = mono_object_get_class(mManagedInstance);
		mNumElements = (UINT32)mono_array_length((MonoArray*)mManagedInstance);

		mArrayEntries.resize(mNumElements);
		for(UINT32 i = 0; i < mNumElements; i++)
		{
			mArrayEntries[i] = getFieldData(i);
		}
	}

	ScriptSerializableArrayPtr ScriptSerializableArray::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoArrayPtr& typeInfo)
	{
		if(!RuntimeScriptObjects::instance().isArray(managedInstance))
			return nullptr;

		return cm_shared_ptr<ScriptSerializableArray>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ScriptSerializableArrayPtr ScriptSerializableArray::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableArray>(ConstructPrivately());
	}

	void ScriptSerializableArray::serializeManagedInstance()
	{
		mArrayEntries.resize(mNumElements);
		for(UINT32 i = 0; i < mNumElements; i++)
		{
			mArrayEntries[i] = getFieldData(i);
		}
	}

	void ScriptSerializableArray::deserializeManagedInstance()
	{
		if(!mArrayTypeInfo->isTypeLoaded())
			return;

		uint32_t lengths[1] = { mNumElements };

		MonoArray* newArray = mono_array_new_full(MonoManager::instance().getDomain(), 
			mArrayTypeInfo->getMonoClass(), (uintptr_t*)lengths, nullptr); 

		mManagedInstance = (MonoObject*)newArray;

		CM::UINT32 idx = 0;
		for(auto& arrayEntry : mArrayEntries)
		{
			setFieldData(idx, arrayEntry);
		}
	}

	void ScriptSerializableArray::setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		setValue(arrayIdx, val->getValue(mArrayTypeInfo->mElementType));
	}

	ScriptSerializableFieldDataPtr ScriptSerializableArray::getFieldData(CM::UINT32 arrayIdx)
	{
		return ScriptSerializableFieldData::create(mArrayTypeInfo->mElementType, getValue(arrayIdx));
	}
	
	void ScriptSerializableArray::setValue(CM::UINT32 arrayIdx, void* val)
	{
		if(arrayIdx >= mNumElements)
			CM_EXCEPT(InvalidParametersException, "Array index out of range: " + toString(arrayIdx) + ". Valid range is [0, " + toString(mNumElements) + ")");

		MonoArray* array = (MonoArray*)mManagedInstance;
		UINT32 elemSize = mono_array_element_size(mClass);
	
		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);
	
		void* elemAddr = mono_array_addr_with_size(array, elemSize, arrayIdx);
		memcpy(elemAddr, val, elemSize);
	}
	
	void* ScriptSerializableArray::getValue(CM::UINT32 arrayIdx)
	{
		if(arrayIdx >= mNumElements)
			CM_EXCEPT(InvalidParametersException, "Array index out of range: " + toString(arrayIdx) + ". Valid range is [0, " + toString(mNumElements) + ")");

		MonoArray* array = (MonoArray*)mManagedInstance;
		UINT32 elemSize = mono_array_element_size(mClass);
	
		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);
	
		return mono_array_addr_with_size(array, elemSize, arrayIdx);
	}

	RTTITypeBase* ScriptSerializableArray::getRTTIStatic()
	{
		return ScriptSerializableArrayRTTI::instance();
	}

	RTTITypeBase* ScriptSerializableArray::getRTTI() const
	{
		return ScriptSerializableArray::getRTTIStatic();
	}
}