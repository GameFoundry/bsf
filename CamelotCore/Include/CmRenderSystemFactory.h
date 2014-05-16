#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Factory class that you may specialize in order to start up
	 *			a specific render system.
	 */
	class RenderSystemFactory
	{
	public:
		/**
		 * @brief	Creates and starts up the render system managed by this factory. 
		 */
		virtual void create() = 0;

		/**
		 * @brief	Returns the name of the render system this factory creates.
		 */
		virtual const String& name() const = 0;
	};
}