#include "CmGLRenderSystemFactory.h"

namespace CamelotEngine
{
	RenderSystemPtr GLRenderSystemFactory::create()
	{
		return RenderSystemPtr(new Ogre::GLRenderSystem());
	}

	GLRenderSystemFactory::InitOnStart GLRenderSystemFactory::initOnStart;
}