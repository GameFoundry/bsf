//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Renderer-Engine
	 *  @{
	 */

	/**
	 * Abstract class that provides an interface for initializing renderable elements of a certain type, along with their
	 * GPU parameter buffers.
	 *
	 * Essentially this class holds all the information that is needed for rendering an element of a certain renderable 
	 * type.
	 */
	class BS_EXPORT RenderableHandler
	{
	public:
		virtual ~RenderableHandler() {}

		/**
		 * Initializes the specified renderable element, making it ready to be used. Caller must ensure the renderable
		 * element actually supports this handler.
		 */
		virtual void initializeRenderElem(RenderableElement& element) = 0;

		/**	Binds per object GPU parameter buffers to the provided element. */
		virtual void bindPerObjectBuffers(const RenderableElement& element) = 0;

		/** Binds global GPU parameter buffers used for this Renderable type to the provided element. */
		virtual void bindGlobalBuffers(const RenderableElement& element);
	};

	/** @} */
	/** @endcond */
}