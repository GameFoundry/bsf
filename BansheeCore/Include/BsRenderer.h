#pragma once

#include "BsCorePrerequisites.h"
#include "BsRendererParams.h"
#include "BsGameObject.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a basic sent of renderable types that may be supported by a renderer.
	 *			These can be used as a guide and renderer plugins can use their own types as needed.
	 */
	enum RenderableType
	{
		RenType_UnlitUntextured,
		RenType_UnlitTextured
		// TODO - Add more types as I improve the Renderer with advanced techniques
	};

	/**
	 * @brief	Primarily rendering class that allows you to specify how to render objects that exist
	 *			in the scene graph. You need to provide your own implementation of your class.
	 *
	 * @note	Normally you would iterate over all cameras, find visible objects for each camera and render
	 *			those objects in some way.
	 */
	class BS_CORE_EXPORT Renderer
	{
	public:
		/**
		 * @brief	Name of the renderer. Used by materials to find 
		 * 			an appropriate technique for this renderer.
		 */
		virtual const String& getName() const = 0;

		/**
		 * @brief	Called in order to render all currently active cameras.
		 */
		virtual void renderAll() = 0;

		/**
		 * @brief	Allows you to register a callback for the specified viewport. The callback
		 * 			will be called before rendering and you will be able to populate the
		 * 			render queue with render commands that will be executed when rendering.
		 */
		void addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, DrawList&)> callback);

	protected:
		UnorderedMap<const Viewport*, Vector<std::function<void(const Viewport*, DrawList&)>>> mRenderCallbacks;
		UnorderedSet<RendererMaterialParams> mRenderableMaterialParams;
	};
}