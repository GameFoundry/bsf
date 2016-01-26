#include "BsGLRenderSystemFactory.h"
#include "BsGLRenderSystem.h"

namespace BansheeEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp<GLRenderSystem>();
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}