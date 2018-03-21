//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Reflection/BsIReflectable.h"
#include "Reflection/BsRTTIType.h"
#include "Error/BsException.h"
#include "Private/RTTI/BsIReflectableRTTI.h"

namespace bs
{
	void IReflectable::_registerDerivedClass(RTTITypeBase* derivedClass)
	{
		if(_isTypeIdDuplicate(derivedClass->getRTTIId()))
		{
			BS_EXCEPT(InternalErrorException, "RTTI type \"" + derivedClass->getRTTIName() + 
				"\" has a duplicate ID: " + toString(derivedClass->getRTTIId()));
		}

		getDerivedClasses().push_back(derivedClass);
	}

	SPtr<IReflectable> IReflectable::createInstanceFromTypeId(UINT32 rttiTypeId)
	{
		RTTITypeBase* type = _getRTTIfromTypeId(rttiTypeId);

		SPtr<IReflectable> output;
		if(type != nullptr)
		{
			output = type->newRTTIObject();
			output->mRTTIData = nullptr;
		}
		
		return output;
	}

	RTTITypeBase* IReflectable::_getRTTIfromTypeId(UINT32 rttiTypeId)
	{
		Stack<RTTITypeBase*> todo;

		for(const auto & item : getDerivedClasses())
			todo.push(item);

		while(!todo.empty())
		{
			RTTITypeBase* curType = todo.top();
			todo.pop();

			if(curType->getRTTIId() == rttiTypeId)
				return curType;

			for(const auto & item : curType->getDerivedClasses())
				todo.push(item);
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
		return getRTTI()->isDerivedFrom(base);
	}

	void IReflectable::_checkForCircularReferences()
	{
		Stack<RTTITypeBase*> todo;

		Vector<RTTITypeBase*> rootTypes = getDerivedClasses();
		for (auto& entry : rootTypes)
			todo.push(entry);

		while(!todo.empty())
		{
			RTTITypeBase* myType = todo.top();
			todo.pop();

			UINT32 myNumFields = myType->getNumFields();
			for (UINT32 i = 0; i < myNumFields; i++)
			{
				RTTIField* myField = myType->getField(i);

				if (!myField->isReflectablePtrType())
					continue;

				RTTIReflectablePtrFieldBase* myReflectablePtrField = static_cast<RTTIReflectablePtrFieldBase*>(myField);
				
				RTTITypeBase* otherType = myReflectablePtrField->getType();
				UINT32 otherNumFields = otherType->getNumFields();
				for (UINT32 j = 0; j < otherNumFields; j++)
				{
					RTTIField* otherField = otherType->getField(j);

					if (!otherField->isReflectablePtrType())
						continue;

					RTTIReflectablePtrFieldBase* otherReflectablePtrField = static_cast<RTTIReflectablePtrFieldBase*>(otherField);

					if (myType->getRTTIId() == otherReflectablePtrField->getType()->getRTTIId() &&
						(myReflectablePtrField->getFlags() & RTTI_Flag_WeakRef) == 0 &&
						(otherReflectablePtrField->getFlags() & RTTI_Flag_WeakRef) == 0)
					{
						BS_EXCEPT(InternalErrorException, "Found circular reference on RTTI type: " + myType->getRTTIName()
							+ " to type: " + otherType->getRTTIName() + ". Either remove one of the references or mark it" 
							+ " as a weak reference when defining the RTTI field.");
					}
				}
			}
		}
	}

	UINT32 IReflectable::getTypeId() const
	{ 
		return getRTTI()->getRTTIId(); 
	}

	const String& IReflectable::getTypeName() const
	{
		return getRTTI()->getRTTIName();
	}

	RTTITypeBase* IReflectable::getRTTIStatic()
	{
		return IReflectableRTTI::instance();
	}
}
