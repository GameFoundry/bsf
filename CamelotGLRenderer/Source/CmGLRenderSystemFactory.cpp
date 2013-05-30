#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace CamelotFramework
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp(cm_new<GLRenderSystem>());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}