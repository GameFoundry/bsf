#include "CmForwardRendererFactory.h"
#include "CmRenderer.h"
#include "CmForwardRenderer.h"

namespace CamelotEngine
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