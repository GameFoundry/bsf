//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGLContext.h"

namespace bs::ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	MacOS specific implementation of an OpenGL context. */
	class MacOSContext : public GLContext
	{
		struct Pimpl;
	public:
		/**
		 * Constructs a new OpenGL context.
		 *
		 * @param[in]	depthStencil	True if the framebuffer using the context can have a depth-stencil buffer.
		 * @param[in]	msaaCount		Number of samples the framebuffer using the context is allowed to have.
		 */
		MacOSContext(bool depthStencil, UINT32 msaaCount);
		virtual ~MacOSContext();

		/** @copydoc GLContext::setCurrent */
		void setCurrent(const RenderWindow& window) override;

		/** @copydoc GLContext::endCurrent */
		void endCurrent() override;

		/** @copydoc GLContext::releaseContext  */
		void releaseContext() override;

		/** Marks the context as dirty and requiring update. Should be called when the drawable changes size or location. */
		void markAsDirty();

		/** Updates the context if dirty. */
		void updateIfDirty();

		/** Enables or disables VSync using the specified interval. Interval of 0 disables VSync. */
		void setVSync(int interval);

		/** Swaps the framebuffer currently attached to this context. */
		void swapBuffers();

	private:
		Pimpl* m;
	};

	/** @} */
}

