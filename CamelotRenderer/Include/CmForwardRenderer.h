#pragma once

#include "CmPrerequisites.h"
#include "CmRenderer.h"

namespace CamelotEngine
{
	class CM_EXPORT ForwardRenderer : public Renderer
	{
	public:
		/**
		 * @brief	Overriden from Renderer
		 */
		virtual const String& getName() const;

		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void renderAll();

		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void render(const CameraPtr camera);

	protected:
		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void setPass(const Pass* pass);
	};
}