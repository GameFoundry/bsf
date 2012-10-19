#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class RenderManager
	{
	public:
		/**
		 * @brief	Renders all cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	 Renders the scene from the perspective of a single camera
		 */
		virtual void render(const CameraPtr camera) = 0;
	};
}