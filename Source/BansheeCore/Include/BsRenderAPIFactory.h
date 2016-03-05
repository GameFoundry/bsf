//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Factory class that you may specialize in order to start up a specific render system. */
	class RenderAPIFactory
	{
	public:
		/**	Creates and starts up the render system managed by this factory.  */
		virtual void create() = 0;

		/**	Returns the name of the render system this factory creates. */
		virtual const String& name() const = 0;
	};

	/** @} */
	/** @endcond */
}