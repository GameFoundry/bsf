#include "CmIReflectable.h"
#include "CmRTTIType.h"
#include "CmException.h"

namespace BansheeEngine
{
	void IReflectable::_registerDerivedClass(RTTITypeBase* derivedClass)
	{
		if(_isTypeIdDuplicate(derivedClass->getRTTIId()))
		{
			CM_EXCEPT(InternalErrorException, "RTTI type \"" + derivedClass->getRTTIName() + 
				"\" has a duplicate ID: " + toString(derivedClass->getRTTIId()));
		}

		getDerivedClasses().push_back(derivedClass);
	}

	std::shared_ptr<IReflectable> IReflectable::createInstanceFromTypeId(UINT32 rttiTypeId)
	{
		RTTITypeBase* type = _getRTTIfromTypeId(rttiTypeId);

		if(type != nullptr)
			return type->newRTTIObject();
		
		return nullptr;
	}

	RTTITypeBase* IReflectable::_getRTTIfromTypeId(UINT32 rttiTypeId)
	{
		Stack<RTTITypeBase*>::type todo;
		Vector<RTTITypeBase*>::type& rootClasses = getDerivedClasses();

		for(auto iter = rootClasses.begin(); iter != rootClasses.end(); ++iter)
			todo.push(*iter);

		while(!todo.empty())
		{
			RTTITypeBase* curType = todo.top();
			todo.pop();

			if(curType->getRTTIId() == rttiTypeId)
				return curType;

			Vector<RTTITypeBase*>::type& derivedClasses = curType->getDerivedClasses();
			for(auto iter = derivedClasses.begin(); iter != derivedClasses.end(); ++iter)
				todo.push(*iter);
		}

		return nullptr;
	}

	bool IReflectable::_isTypeIdDuplicate(UINT32 typeId)
	{
		if(typeId == TID_Abstract)
			return false;

		return IReflectable::_getRTTIfromTypeId(typeId) != nullptr;
	}

	bool IReflectable::isDerivedFrom(RTTITypeBase* base)
	{
		assert(base != nullptr);

		Stack<RTTITypeBase*>::type todo;
		todo.push(base);

		while (!todo.empty())
		{
			RTTITypeBase* currentType = todo.top();
			todo.pop();

			if(currentType->getRTTIId() == getRTTI()->getRTTIId())
				return true;

			Vector<RTTITypeBase*>::type& derivedClasses = currentType->getDerivedClasses();
			for(auto iter = derivedClasses.begin(); iter != derivedClasses.end(); ++iter)
				todo.push(*iter);
		}

		return false;
	}

	UINT32 IReflectable::getTypeId() const
	{ 
		return getRTTI()->getRTTIId(); 
	}

	const String& IReflectable::getTypeName() const
	{
		return getRTTI()->getRTTIName();
	}
}