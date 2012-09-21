#include "CmRTTIType.h"
#include "CmException.h"

namespace CamelotEngine
{
	RTTITypeBase::RTTITypeBase()
	{ }

	RTTITypeBase::~RTTITypeBase() 
	{
		for(auto iter = mFields.begin(); iter != mFields.end(); ++iter)
			delete *iter;

		mFields.clear();
	}

	RTTIField* RTTITypeBase::findField(const std::string& name)
	{
		auto foundElement = std::find_if(mFields.begin(), mFields.end(), [&name](RTTIField* x) { return x->mName == name; });

		if(foundElement == mFields.end())
		{
			CM_EXCEPT(InternalErrorException, 
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
			CM_EXCEPT(InvalidParametersException, 
				"Field argument can't be null.");
		}

		int uniqueId = field->mUniqueId;
		auto foundElementById = std::find_if(mFields.begin(), mFields.end(), [uniqueId](RTTIField* x) { return x->mUniqueId == uniqueId; });

		if(foundElementById != mFields.end())
		{
			CM_EXCEPT(InternalErrorException, 
				"Field with the same ID already exists.");
		}

		std::string& name = field->mName;
		auto foundElementByName = std::find_if(mFields.begin(), mFields.end(), [&name](RTTIField* x) { return x->mName == name; });

		if(foundElementByName != mFields.end())
		{
			CM_EXCEPT(InternalErrorException, 
				"Field with the same name already exists.");
		}

		mFields.push_back(field);
	}
}