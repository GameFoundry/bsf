#include "CmIReflectable.h"
#include "CmException.h"

namespace CamelotEngine
{
	IReflectable* IReflectable::newObject() 
	{ 
		CM_EXCEPT(InternalErrorException, 
			"Cannot create an instance of an abstract class.");

		return nullptr; 
	} 
}