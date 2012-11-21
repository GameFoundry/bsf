#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class Renderer
	{
	public:
		virtual const String& getName() const = 0;

		/**
		 * @brief	Renders all cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	 Renders the scene from the perspective of a single camera
		 */
		virtual void render(const CameraPtr camera) = 0;

	protected:
		/**
		 * @brief	Sets the currently active pass.
		 */
		virtual void setPass(const PassPtr pass) = 0;
	};
}