#pragma once

#include "CmForwardRendererPrerequisites.h"
#include "CmRenderer.h"

namespace CamelotEngine
{
	class CM_FWDRND_EXPORT ForwardRenderer : public Renderer
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		virtual const String& getName() const;

		virtual void renderAll();
		virtual void render(const CameraPtr camera);

	protected:
		PassPtr mActivePass;
		RenderCommandBuffer* mCommandBuffer;

		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void setPass(PassPtr pass);

		/**
		 * @brief	Overriden from Renderer
		 */
		virtual void setPassParameters(PassParametersPtr params);
	};
}