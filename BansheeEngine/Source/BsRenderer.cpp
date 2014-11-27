#include "BsRenderer.h"
#include "BsRendererManager.h"

namespace BansheeEngine
{
	SPtr<Renderer> gRenderer()
	{
		return std::static_pointer_cast<Renderer>(RendererManager::instance().getActive());
	}
}