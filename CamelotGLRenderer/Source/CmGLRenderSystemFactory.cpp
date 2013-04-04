#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace CamelotEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp(CM_NEW(GLRenderSystem, GenAlloc) GLRenderSystem());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}