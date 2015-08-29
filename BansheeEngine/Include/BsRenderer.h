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
		virtual void _notifyRenderableAdded(RenderableCore* renderable) { }

		/**
		 * @brief	Called whenever a renderable is updated.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyRenderableUpdated(RenderableCore* renderable) { }

		/**
		 * @brief	Called whenever a renderable is destroyed.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyRenderableRemoved(RenderableCore* renderable) { }

		/**
		 * @brief	Called whenever a new light is created.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyLightAdded(LightCore* light) { }

		/**
		 * @brief	Called whenever a light is updated.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyLightUpdated(LightCore* light) { }

		/**
		 * @brief	Called whenever a light is destroyed.
		 *
		 * @note	Core thread.
		 *			Internal method.
		 */
		virtual void _notifyLightRemoved(LightCore* light) { }
	};

	/**
	 * @brief	Retrieves the currently active renderer.
	 */
	SPtr<Renderer> BS_EXPORT gRenderer();
}