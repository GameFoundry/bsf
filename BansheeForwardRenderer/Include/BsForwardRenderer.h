#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRenderer.h"

namespace BansheeEngine
{
	class BS_FWDRND_EXPORT ForwardRenderer : public Renderer
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		virtual const String& getName() const;

		virtual void renderAll();
		virtual void render(const HCamera& camera);

	private:
		RenderQueue* mRenderQueue; // TODO - Move this to base class
	};
}