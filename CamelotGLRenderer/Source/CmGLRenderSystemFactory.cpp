#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace CamelotFramework
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp<GLRenderSystem>();
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}