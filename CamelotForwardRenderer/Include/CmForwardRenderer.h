#pragma once

#include "CmForwardRendererPrerequisites.h"
#include "CmRenderer.h"

namespace CamelotEngine
{
	class CM_FWDRND_EXPORT ForwardRenderer : public Renderer
	{
	public:
		virtual const String& getName() const;

		virtual void renderAll();
		virtual void render(CameraPtr camera);

	protected:
		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void setPass(const PassPtr pass);
	};
}