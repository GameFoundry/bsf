#pragma once

#include "BsPrerequisites.h"
#include "BsCoreRenderer.h"

namespace BansheeEngine
{
	/**
	 * @copydoc	CoreRenderer
	 */
	class BS_EXPORT Renderer : public CoreRenderer
	{
	public:
		virtual ~Renderer() { }

		/**
		 * @brief	Called whenever a new renderable is created.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyRenderableAdded(RenderableHandlerCore* renderable) { }

		/**
		 * @brief	Called whenever a renderable is updated.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyRenderableUpdated(RenderableHandlerCore* renderable) { }

		/**
		 * @brief	Called whenever a renderable is destroyed.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyRenderableRemoved(RenderableHandlerCore* renderable) { }
	};

	/**
	 * @brief	Retrieves the currently active renderer.
	 */
	SPtr<Renderer> BS_EXPORT gRenderer();
}