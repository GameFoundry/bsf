#include "BsRenderable.h"
#include "BsRenderableRTTI.h"

using namespace CamelotFramework;

namespace BansheeEngine
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