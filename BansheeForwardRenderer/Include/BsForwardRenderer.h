#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRenderer.h"

namespace BansheeEngine
{
	/**
	 * @brief	Default renderer for Banshee. Performs frustum culling, sorting and renders
	 *			objects plainly according to their shaders with no fancy effects.
	 *
	 * TODO - Update doc when I implement this properly
	 */
	class BS_FWDRND_EXPORT ForwardRenderer : public Renderer
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		/**
		 * @copydoc	Renderer::getName
		 */
		virtual const String& getName() const;

		/**
		 * @copydoc	Renderer::renderAll
		 */
		virtual void renderAll();

		/**
		 * @brief	Renders all objects visible by the provided camera.
		 */
		virtual void render(const HCamera& camera);

	private:
		RenderQueue* mRenderQueue; // TODO - Move this to base class
	};
}