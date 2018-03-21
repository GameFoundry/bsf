//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Reflection/BsRTTIType.h"
#include "Error/BsException.h"

namespace bs
{
	RTTITypeBase::~RTTITypeBase() 
	{
		for(const auto & item : mFields)
			bs_delete(item);
	}

	RTTIField* RTTITypeBase::findField(const String& name)
	{
		auto foundElement = std::find_if(mFields.begin(), mFields.end(), [&name](RTTIField* x) { return x->mName == name; });

		if(foundElement == mFields.end())
		{
			BS_EXCEPT(InternalErrorException, 
				"Cannot find a field with the specified name: " + name);
		}

		return *foundElement;
	}

	RTTIField* RTTITypeBase::findField(int uniqueFieldId)
	{
		auto foundElement = std::find_if(mFields.begin(), mFields.end(), [&uniqueFieldId](RTTIField* x) { return x->mUniqueId == uniqueFieldId; });

		if(foundElement == mFields.end())
			return nullptr;

		return *foundElement;
	}

	void RTTITypeBase::addNewField(RTTIField* field)
	{
		if(field == nullptr)
		{
			BS_EXCEPT(InvalidParametersException, 
				"Field argument can't be null.");
		}

		int uniqueId = field->mUniqueId;
		auto foundElementById = std::find_if(mFields.begin(), mFields.end(), [uniqueId](RTTIField* x) { return x->mUniqueId == uniqueId; });

		if(foundElementById != mFields.end())
		{
			BS_EXCEPT(InternalErrorException, 
				"Field with the same ID already exists.");
		}

		String& name = field->mName;
		auto foundElementByName = std::find_if(mFields.begin(), mFields.end(), [&name](RTTIField* x) { return x->mName == name; });

		if(foundElementByName != mFields.end())
		{
			BS_EXCEPT(InternalErrorException, 
				"Field with the same name already exists.");
		}

		mFields.push_back(field);
	}

	SPtr<IReflectable> rtti_create(UINT32 rttiId)
	{
		return IReflectable::createInstanceFromTypeId(rttiId);
	}
}
