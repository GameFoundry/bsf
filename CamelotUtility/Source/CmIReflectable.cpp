#include "CmIReflectable.h"
#include "CmRTTIType.h"
#include "CmException.h"

namespace CamelotEngine
{
	void IReflectable::registerDerivedClass(RTTITypeBase* derivedClass)
	{
		if(isTypeIdDuplicate(derivedClass->getRTTIId()))
		{
			CM_EXCEPT(InternalErrorException, "RTTI type \"" + derivedClass->getRTTIName() + 
				"\" has a duplicate ID: " + toString(derivedClass->getRTTIId()));
		}

		getDerivedClasses().push_back(derivedClass);
	}

	std::shared_ptr<IReflectable> IReflectable::createInstanceFromTypeId(UINT32 rttiTypeId)
	{
		RTTITypeBase* type = getRTTIfromTypeId(rttiTypeId);

		if(type != nullptr)
			return type->newRTTIObject();
		
		return nullptr;
	}

	RTTITypeBase* IReflectable::getRTTIfromTypeId(UINT32 rttiTypeId)
	{
		stack<RTTITypeBase*>::type todo;
		vector<RTTITypeBase*>::type& rootClasses = getDerivedClasses();

		for(auto iter = rootClasses.begin(); iter != rootClasses.end(); ++iter)
			todo.push(*iter);

		while(!todo.empty())
		{
			RTTITypeBase* curType = todo.top();
			todo.pop();

			if(curType->getRTTIId() == rttiTypeId)
				return curType;

			vector<RTTITypeBase*>::type& derivedClasses = curType->getDerivedClasses();
			for(auto iter = derivedClasses.begin(); iter != derivedClasses.end(); ++iter)
				todo.push(*iter);
		}

		return nullptr;
	}

	bool IReflectable::isTypeIdDuplicate(UINT32 typeId)
	{
		return IReflectable::getRTTIfromTypeId(typeId) != nullptr;
	}
}