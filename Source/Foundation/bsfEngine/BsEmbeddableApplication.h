//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsApplication.h"

namespace bs
{
	/** @addtogroup Application-Engine
	 *	@{
	 */

	/**	Primary entry point for the framework. Handles startup and shutdown.
	 *  Variant that supports having an external main loop and other features needed
	 *  for embedding the framework in other software.
	 */
	class BS_EXPORT EmbeddableApplication : public Application
	{
	public:
		EmbeddableApplication(const START_UP_DESC& desc);
		virtual ~EmbeddableApplication();

		// /** Starts the framework. If using a custom Application system, provide it as a template parameter. */
		// template<class T = Application>
		// static void startUp(VideoMode videoMode, const String& title, bool fullscreen)
		// {
		//	START_UP_DESC desc = buildStartUpDesc(videoMode, title, fullscreen);
		//	CoreApplication::startUp<T>(desc);
		// }

		/** Starts the framework. If using a custom Application system, provide it as a template parameter. */
		template<class T = Application>
		static void startUp(const START_UP_DESC& desc)
		{
			CoreApplication::startUp<T>(desc);
		}

		/** Runs logic and renders a single frame */
		virtual void renderFrame();

		/** Waits until current frame has ended. This needs to be called when certain operations are done. */
		virtual void waitUntilFrameFinished();
	protected:
		/** @copydoc Module::onShutDown */
		void onShutDown() override;
	};

	/** Easy way to access EmbeddableApplication. */
	BS_EXPORT EmbeddableApplication& gEmbeddableApplication();

	/** @} */
}
