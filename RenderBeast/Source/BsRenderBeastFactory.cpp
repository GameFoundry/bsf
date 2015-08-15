#include "BsRenderBeastFactory.h"
#include "BsRenderer.h"
#include "BsRenderBeast.h"

namespace BansheeEngine
{
	CoreRendererPtr RenderBeastFactory::create()
	{
		return bs_shared_ptr_new<RenderBeast>();
	}

	const String& RenderBeastFactory::name() const
	{
		return SystemName;
	}
}