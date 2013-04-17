#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRenderer.h"

namespace BansheeEngine
{
	class BS_FWDRND_EXPORT ForwardRenderer : public CM::Renderer
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		virtual const CM::String& getName() const;

		virtual void renderAll();
		virtual void render(const HCamera& camera);

	protected:
		CM::RenderCommandBuffer* mCommandBuffer;
	};
}