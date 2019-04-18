//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	/** @addtogroup Renderer-Internal
	 *  @{
	 */

	/**
	 * Factory class for creating Renderer objects. Implement this class for any custom renderer classes you may have, and 
	 * register it with renderer manager.
	 * 			
	 * @see		RendererManager
	 */
	class BS_CORE_EXPORT RendererFactory
	{
	public:
		virtual ~RendererFactory() = default;

		/**	Creates a new instance of the renderer. */
		virtual SPtr<ct::Renderer> create() = 0;

		/**	Returns the name of the renderer this factory creates. */
		virtual const String& name() const = 0;
	};

	/** @} */
}