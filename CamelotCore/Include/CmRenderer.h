#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	class Renderer
	{
	public:
		virtual const String& getName() const = 0;

		/**
		 * @brief	Renders all cameras.
		 */
		virtual void renderAll() = 0;
	};
}