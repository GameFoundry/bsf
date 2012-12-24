#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace CamelotEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp(new GLRenderSystem());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}