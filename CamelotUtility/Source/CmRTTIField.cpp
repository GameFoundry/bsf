#include "CmRTTIField.h"
#include "CmException.h"

namespace CamelotEngine
{
	void RTTIField::checkIsSimple(bool array)
	{
		if(!isSimpleType())
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Simple type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isSimpleType()) + ", " + toString(isComplexType()) + ", " + toString(isDataBlockType()) + ", " + toString(isComplexPtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsDataBlock()
	{
		if(!isDataBlockType())
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Data block. Got: " + toString(mIsVectorType) + ", " + 
				toString(isSimpleType()) + ", " + toString(isComplexType()) + ", " + toString(isDataBlockType()) + ", " + toString(isComplexPtrType()));
		}
	}

	void RTTIField::checkIsComplex(bool array)
	{
		if(!isComplexType())
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Complex type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isSimpleType()) + ", " + toString(isComplexType()) + ", " + toString(isDataBlockType()) + ", " + toString(isComplexPtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsComplexPtr(bool array)
	{
		if(!isComplexPtrType())
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed: Complex ptr type. Got: " + toString(mIsVectorType) + ", " + 
				toString(isSimpleType()) + ", " + toString(isComplexType()) + ", " + toString(isDataBlockType()) + ", " + toString(isComplexPtrType()));
		}

		checkIsArray(array);
	}

	void RTTIField::checkIsArray(bool array)
	{
		if(array && !mIsVectorType)
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed an array type but got a single type.");
		}

		if(!array && mIsVectorType)
		{
			CM_EXCEPT(InternalErrorException, 
				"Invalid field type. Needed a single type but got an array type.");
		}
	}
}