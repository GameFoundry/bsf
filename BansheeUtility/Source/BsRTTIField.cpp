//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsRTTIField.h"
#include "BsException.h"

namespace BansheeEngine
{
	void RTTIField::checkIsPlain(bool array)
	{
		if(!isPlainType())
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Plain type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isPlainType()) + ", " + toString(isReflectableType()) + ", " + toString(isDataBlockType()) + ", " + toString(isReflectablePtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsDataBlock()
	{
		if(!isDataBlockType())
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Data block. Got: " + toString(mIsVectorType) + ", " + 
				toString(isPlainType()) + ", " + toString(isReflectableType()) + ", " + toString(isDataBlockType()) + ", " + toString(isReflectablePtrType()));
		}
	}

	void RTTIField::checkIsComplex(bool array)
	{
		if(!isReflectableType())
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Complex type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isPlainType()) + ", " + toString(isReflectableType()) + ", " + toString(isDataBlockType()) + ", " + toString(isReflectablePtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsComplexPtr(bool array)
	{
		if(!isReflectablePtrType())
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Complex ptr type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isPlainType()) + ", " + toString(isReflectableType()) + ", " + toString(isDataBlockType()) + ", " + toString(isReflectablePtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsArray(bool array)
	{
		if(array && !mIsVectorType)
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed an array type but got a single type.");
		}

		if(!array && mIsVectorType)
		{
			BS_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed a single type but got an array type.");
		}
	}
}