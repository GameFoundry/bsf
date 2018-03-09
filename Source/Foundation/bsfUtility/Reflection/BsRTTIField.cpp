//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Reflection/BsRTTIField.h"
#include "Error/BsException.h"

namespace bs
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