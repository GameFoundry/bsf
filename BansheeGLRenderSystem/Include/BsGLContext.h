//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsGLPrerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Abstract class that encapsulated an OpenGL context. Each platform
	 *			should provide its own GLContext specialization.
	 */
    class BS_RSGL_EXPORT GLContext
    {
    public:
        GLContext();
        virtual ~GLContext();

		/**
		 * @brief	Activates the rendering context (all subsequent rendering commands will be executed on it).
		 */
        virtual void setCurrent() = 0;

		/**
		 * @brief	Deactivates the rendering context. Normally called just before setCurrent is called on another context.
		 */
        virtual void endCurrent() = 0;
        
		/**
		 * @brief	Releases the render context, freeing all of its resources.
		 */
		virtual void releaseContext() {}
    };
}
