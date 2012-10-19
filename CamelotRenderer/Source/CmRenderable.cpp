#include "CmRenderable.h"
#include "CmRenderableRTTI.h"

namespace CamelotEngine
{
	RTTITypeBase* Renderable::getRTTIStatic()
	{
		return RenderableRTTI::instance();
	}

	RTTITypeBase* Renderable::getRTTI() const
	{
		return Renderable::getRTTIStatic();
	}
}