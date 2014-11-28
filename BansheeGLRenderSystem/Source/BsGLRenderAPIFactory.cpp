#include "BsGLRenderAPIFactory.h"
#include "BsGLRenderAPI.h"

namespace BansheeEngine
{
	void GLRenderAPIFactory::create()
	{
		RenderAPICore::startUp<GLRenderAPI>();
	}

	GLRenderAPIFactory::InitOnStart GLRenderAPIFactory::initOnStart;
}