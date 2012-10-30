#include "CmForwardRendererFactory.h"
#include "CmRenderer.h"
#include "CmForwardRenderer.h"

namespace CamelotEngine
{
	RendererPtr ForwardRendererFactory::create()
	{
		return RendererPtr(new ForwardRenderer());
	}

	const std::string& ForwardRendererFactory::name() const
	{
		return SystemName;
	}
}