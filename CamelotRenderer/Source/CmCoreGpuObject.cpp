#include "CmCoreGpuObject.h"
#include "CmRenderSystem.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	CoreGpuObject::CoreGpuObject()
		:mDestroyed(false)
	{ }

	CoreGpuObject::~CoreGpuObject() 
	{
		if(!mDestroyed)
		{
			LOGWRN("Destructor called but object is not destroyed. Object will leak.")
		}
	}

	void CoreGpuObject::destroy()
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&CoreGpuObject::destroy_internal, this));
	}

	void CoreGpuObject::destroy_internal()
	{
		mDestroyed = true;
	}
}