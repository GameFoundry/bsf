#include "CmIDestroyable.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	IDestroyable::IDestroyable()
		:mDestroyed(false)
	{ }

	IDestroyable::~IDestroyable() 
	{
		if(!mDestroyed)
		{
			LOGWRN("Destructor called but object is not destroyed. Object will leak.")
		}
	}

	void IDestroyable::destroy()
	{
		mDestroyed = true;
	}
}