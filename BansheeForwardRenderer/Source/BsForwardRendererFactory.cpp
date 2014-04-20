#include "BsForwardRendererFactory.h"
#include "CmRenderer.h"
#include "BsForwardRenderer.h"

namespace BansheeEngine
{
	RendererPtr ForwardRendererFactory::create()
	{
		return cm_shared_ptr<ForwardRenderer>();
	}

	const String& ForwardRendererFactory::name() const
	{
		return SystemName;
	}
}