#include "BsBansheeRendererFactory.h"
#include "BsRenderer.h"
#include "BsBansheeRenderer.h"

namespace BansheeEngine
{
	CoreRendererPtr BansheeRendererFactory::create()
	{
		return bs_shared_ptr_new<BansheeRenderer>();
	}

	const String& BansheeRendererFactory::name() const
	{
		return SystemName;
	}
}