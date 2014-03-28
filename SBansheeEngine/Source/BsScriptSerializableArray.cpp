#include "BsScriptSerializableArray.h"
#include "BsScriptSerializableArrayRTTI.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptSerializableField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptSerializableArray::ScriptSerializableArray(const ConstructPrivately& dummy)
		:mManagedInstance(nullptr), mElemSize(0), mElementMonoClass(nullptr)
	{

	}

	ScriptSerializableArray::ScriptSerializableArray(const ConstructPrivately& dummy, const ScriptSerializableTypeInfoArrayPtr& typeInfo, MonoObject* managedInstance)
		:mArrayTypeInfo(typeInfo), mManagedInstance(managedInstance), mElemSize(0), mElementMonoClass(nullptr)
	{
		::MonoClass* monoClass = mono_object_get_class(mManagedInstance);
		mElemSize = mono_array_element_size(monoClass);

		initMonoObjects();

		mNumElements.resize(typeInfo->mRank);
		for(UINT32 i = 0; i < typeInfo->mRank; i++)
			mNumElements[i] = getLength(i);
	}

	ScriptSerializableArrayPtr ScriptSerializableArray::create(MonoObject* managedInstance, const ScriptSerializableTypeInfoArrayPtr& typeInfo)
	{
		if(managedInstance == nullptr)
			return nullptr;

		if(!RuntimeScriptObjects::instance().getSystemArrayClass()->isInstanceOfType(managedInstance))
			return nullptr;

		return cm_shared_ptr<ScriptSerializableArray>(ConstructPrivately(), typeInfo, managedInstance);
	}

	ScriptSerializableArrayPtr ScriptSerializableArray::createEmpty()
	{
		return cm_shared_ptr<ScriptSerializableArray>(ConstructPrivately());
	}

	void ScriptSerializableArray::serializeManagedInstance()
	{
		UINT32 totalNumElements = 1;
		for(auto& numElems : mNumElements)
		{
			totalNumElements *= numElems;
		}

		mArrayEntries.resize(totalNumElements);
		for(UINT32 i = 0; i < totalNumElements; i++)
		{
			mArrayEntries[i] = getFieldData(i);
		}
	}

	void ScriptSerializableArray::deserializeManagedInstance()
	{
		if(!mArrayTypeInfo->isTypeLoaded())
			return;

		MonoClass* arrayClass = RuntimeScriptObjects::instance().getSystemArrayClass();

		MonoMethod* createInstance = arrayClass->getMethodExact("CreateInstance", "Type,int[]");
		MonoArray* lengthArray = mono_array_new(MonoManager::instance().getDomain(), mono_get_int32_class(), (UINT32)mNumElements.size());

		for(UINT32 i = 0; i < (UINT32)mNumElements.size(); i++)
		{
			void* elemAddr = mono_array_addr_with_size(lengthArray, sizeof(int), i);
			memcpy(elemAddr, &mNumElements[i], sizeof(int));
		}

		void* params[2] = { 
			mono_type_get_object(MonoManager::instance().getDomain(), mono_class_get_type(mArrayTypeInfo->getMonoClass())), lengthArray };

		mManagedInstance = createInstance->invoke(nullptr, params);
		initMonoObjects();

		CM::UINT32 idx = 0;
		for(auto& arrayEntry : mArrayEntries)
		{
			setFieldData(idx, arrayEntry);
			idx++;
		}
	}

	void ScriptSerializableArray::setFieldData(CM::UINT32 arrayIdx, const ScriptSerializableFieldDataPtr& val)
	{
		bool isBoxedValueType = false;
		if(rtti_is_of_type<ScriptSerializableTypeInfoObject>(mArrayTypeInfo->mElementType))
		{
			ScriptSerializableTypeInfoObjectPtr objTypeInfo = std::static_pointer_cast<ScriptSerializableTypeInfoObject>(mArrayTypeInfo->mElementType);
			isBoxedValueType = objTypeInfo->mValueType;
		}

		if(isBoxedValueType)
		{
			MonoObject* value = (MonoObject*)val->getValue(mArrayTypeInfo->mElementType);

			if(value != nullptr)
				setValue(arrayIdx, mono_object_unbox(value)); // Value types need to be set as native unboxed types
		}
		else
		{
			if(mArrayTypeInfo->mElementType->isRawType())
				setValue(arrayIdx, val->getValue(mArrayTypeInfo->mElementType));
			else
			{
				MonoObject* ptrToObj = (MonoObject*)val->getValue(mArrayTypeInfo->mElementType);
				setValue(arrayIdx, &ptrToObj);
			}
		}
	}

	ScriptSerializableFieldDataPtr ScriptSerializableArray::getFieldData(CM::UINT32 arrayIdx)
	{
		MonoArray* array = (MonoArray*)mManagedInstance;

		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);

		void* arrayValue = mono_array_addr_with_size(array, mElemSize, arrayIdx);

		if(mArrayTypeInfo->mElementType->isRawType())
		{
			return ScriptSerializableFieldData::create(mArrayTypeInfo->mElementType, arrayValue);
		}
		else
		{
			if(mono_class_is_valuetype(mElementMonoClass))
			{
				void* rawObj = *(void**)arrayValue;
				assert(rawObj != nullptr);

				MonoObject* boxedObj = mono_value_box(MonoManager::instance().getDomain(), mElementMonoClass, rawObj);
				return ScriptSerializableFieldData::create(mArrayTypeInfo->mElementType, &boxedObj);
			}
			else
				return ScriptSerializableFieldData::create(mArrayTypeInfo->mElementType, arrayValue);
		}
	}
	
	void ScriptSerializableArray::setValue(CM::UINT32 arrayIdx, void* val)
	{
		MonoArray* array = (MonoArray*)mManagedInstance;

		UINT32 numElems = (UINT32)mono_array_length(array);
		assert(arrayIdx < numElems);
	
		void* elemAddr = mono_array_addr_with_size(array, mElemSize, arrayIdx);
		memcpy(elemAddr, val, mElemSize);
	}

	void ScriptSerializableArray::initMonoObjects()
	{
		mElementMonoClass = mArrayTypeInfo->mElementType->getMonoClass();
	}

	UINT32 ScriptSerializableArray::toSequentialIdx(const CM::Vector<CM::UINT32>::type& idx) const
	{
		UINT32 mNumDims = (UINT32)mNumElements.size();

		if(idx.size() != mNumDims)
			CM_EXCEPT(InvalidParametersException, "Provided index doesn't have the correct number of dimensions");

		if(mNumElements.size() == 0)
			return 0;

		UINT32 curIdx = 0;
		UINT32 prevDimensionSize = 1;
		
		for(INT32 i = mNumDims - 1; i >= 0; i--)
		{
			curIdx += idx[i] * prevDimensionSize;

			prevDimensionSize *= mNumElements[i];
		}

		return curIdx;
	}

	UINT32 ScriptSerializableArray::getLength(UINT32 dimension) const
	{
		MonoClass* systemArray = RuntimeScriptObjects::instance().getSystemArrayClass();
		MonoMethod& getLength = systemArray->getMethod("GetLength", 1);

		void* params[1] = { &dimension };
		MonoObject* returnObj = getLength.invoke(mManagedInstance, params);

		return *(UINT32*)mono_object_unbox(returnObj);
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