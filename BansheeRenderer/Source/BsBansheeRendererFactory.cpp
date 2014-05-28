#include "BsBansheeRendererFactory.h"
#include "CmRenderer.h"
#include "BsBansheeRenderer.h"

namespace BansheeEngine
{
	RendererPtr BansheeRendererFactory::create()
	{
		return cm_shared_ptr<BansheeRenderer>();
	}

	const String& BansheeRendererFactory::name() const
	{
		return SystemName;
	}
}