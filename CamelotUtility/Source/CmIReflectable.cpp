#include "CmIReflectable.h"
#include "CmRTTIType.h"
#include "CmException.h"

namespace CamelotEngine
{
	std::shared_ptr<IReflectable> IReflectable::createInstanceFromTypeId(UINT32 rttiTypeId)
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
				return curType->newRTTIObject();

			vector<RTTITypeBase*>::type& derivedClasses = curType->getDerivedClasses();
			for(auto iter = derivedClasses.begin(); iter != derivedClasses.end(); ++iter)
				todo.push(*iter);
		}

		return nullptr;
	}
}