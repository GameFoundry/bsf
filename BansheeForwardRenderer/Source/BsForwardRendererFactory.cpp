#include "BsForwardRendererFactory.h"
#include "CmRenderer.h"
#include "BsForwardRenderer.h"

using namespace CamelotEngine;

namespace BansheeEngine
{
	RendererPtr ForwardRendererFactory::create()
	{
		return RendererPtr(CM_NEW(ForwardRenderer, GenAlloc) ForwardRenderer(), &MemAllocDeleter<ForwardRenderer, GenAlloc>::deleter);
	}

	const std::string& ForwardRendererFactory::name() const
	{
		return SystemName;
	}
}