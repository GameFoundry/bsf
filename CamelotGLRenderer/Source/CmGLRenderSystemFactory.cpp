#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace BansheeEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp<GLRenderSystem>();
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}