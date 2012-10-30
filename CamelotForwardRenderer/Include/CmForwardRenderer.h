#pragma once

#include "CmPrerequisites.h"
#include "CmRenderer.h"

namespace CamelotEngine
{
	class CM_FWDRND_EXPORT ForwardRenderer : public Renderer
	{
	public:
		virtual const String& getName() const;

		virtual void renderAll();
		virtual void render(const CameraPtr camera);
	};
}