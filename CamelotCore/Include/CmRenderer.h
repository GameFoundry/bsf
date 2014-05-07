#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to customize how are objects rendered. You need to
	 * 			provide your own implementation of your class.
	 */
	class CM_EXPORT Renderer
	{
	public:
		/**
		 * @brief	Name of the renderer. Used by materials to find 
		 * 			an appropriate technique for this renderer.
		 */
		virtual const String& getName() const = 0;

		/**
		 * @brief	Called in order to render all currentlx active cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	Allows you to register a callback for the specified viewport. The callback
		 * 			will be called before rendering and you will be able to populate the
		 * 			render queue with render commands that will be executed when rendering.
		 */
		void addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, RenderQueue&)> callback);

	protected:
		UnorderedMap<const Viewport*, Vector<std::function<void(const Viewport*, RenderQueue&)>>> mRenderCallbacks;
	};
}