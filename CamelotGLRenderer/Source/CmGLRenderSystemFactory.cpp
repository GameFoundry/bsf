#include "CmGLRenderSystemFactory.h"

namespace CamelotEngine
{
	RenderSystemPtr GLRenderSystemFactory::create()
	{
		return RenderSystemPtr(new GLRenderSystem());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}