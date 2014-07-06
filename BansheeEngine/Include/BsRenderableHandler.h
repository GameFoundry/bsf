#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * Abstract class that provides an interface for initializing
	 * renderable elements of a certain type, along with their GPU
	 * parameter buffers.
	 *
	 * Essentially this class holds all the information that is needed for rendering
	 * an element of a certain renderable type.
	 *
	 * @note	e.g. elements that have shadows would have a RenderableLitShadowHandler
	 *			and elements that can be animated would have RenderableAnimatedHandler,
	 *			or a combination of the two.	
	 */
	class BS_EXPORT RenderableHandler
	{
	public:
		virtual ~RenderableHandler() {}

		/**
		 * @brief	Initializes the specified renderable element, making it ready
		 *			to be used. Caller must ensure the renderable element actually
		 *			supports this handler.
		 */
		virtual void initializeRenderElem(RenderableElement* element) = 0;

		/**
		 * @brief	Binds per object GPU parameter buffers to the provided element.
		 */
		virtual void bindPerObjectBuffers(const RenderableElement* element) = 0;

		/**
		 * @brief	Binds global GPU parameter buffers used for this Renderable 
		 *			type to the provided element.
		 */
		virtual void bindGlobalBuffers(const RenderableElement* element);
	};
}