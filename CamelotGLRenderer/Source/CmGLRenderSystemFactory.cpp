#include "CmGLRenderSystemFactory.h"
#include "CmGLRenderSystem.h"

namespace BansheeEngine
{
	void GLRenderSystemFactory::create()
	{
		RenderSystem::startUp(cm_new<GLRenderSystem>());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}