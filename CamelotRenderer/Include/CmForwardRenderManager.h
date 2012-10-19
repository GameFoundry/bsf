#pragma once

#include "CmPrerequisites.h"
#include "CmRenderManager.h"

namespace CamelotEngine
{
	class ForwardRenderManager : public RenderManager
	{
	public:
		virtual void renderAll();

		virtual void render(const CameraPtr camera);
	};
}