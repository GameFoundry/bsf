#include "CmRenderable.h"
#include "CmRenderableRTTI.h"

namespace CamelotEngine
{
	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent)
	{ }

	RTTITypeBase* Renderable::getRTTIStatic()
	{
		return RenderableRTTI::instance();
	}

	RTTITypeBase* Renderable::getRTTI() const
	{
		return Renderable::getRTTIStatic();
	}
}