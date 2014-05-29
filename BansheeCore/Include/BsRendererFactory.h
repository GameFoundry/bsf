#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Factory class for creating Renderer objects. Implement
	 * 			this class for any custom renderer classes you may have,
	 * 			and register it with renderer manager.
	 * 			
	 * @see		RendererManager
	 * 			
	 * @note	Internal class.
	 */
	class BS_CORE_EXPORT RendererFactory
	{
	public:
		/**
		 * @brief	Creates a new instance of the renderer.
		 */
		virtual RendererPtr create() = 0;

		/**
		 * @brief	Returns the name of the renderer this factory creates.
		 */
		virtual const String& name() const = 0;
	};
}