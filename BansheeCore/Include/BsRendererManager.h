#pragma once

#include "BsCorePrerequisites.h"
#include "BsRendererFactory.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to change and retrieve the active renderer. Active renderer will
	 * 			be used for rendering all objects in the following frame.
	 * 			
	 * @note	No renderer is active by default. You must make a renderer active before doing any rendering.
	 */
	class BS_CORE_EXPORT RendererManager : public Module<RendererManager>
	{
	public:
		/**
		 * @brief	Attempts to find a renderer with the specified name and makes it active.
		 * 			Exception is thrown if renderer with the specified name doesn't exist.
		 */
		void setActive(const String& name);

		/**
		 * @brief	Returns the currently active renderer. Null if no renderer is active.
		 */
		RendererPtr getActive() { return mActiveRenderer; }

		/**
		 * @brief	Core renderer represents a set of shared features within all renderers.
		 * 			Techniques using this renderer name will report as if they are supported regardless
		 * 			of the active renderer.
		 *
		 * @note	Useful when you want to make a technique working on all renderers. (Normally techniques
		 * 			need to be different as far as render system is concerned but can often be same from
		 * 			renderers perspective).
		 * 			
		 * @see		Technique
		 */
		static const String& getCoreRendererName();

		/**
		 * @brief	Registers a new renderer factory. Any renderer you try to make active with
		 * 			"setActive" you will need to have previously registered here.
		 *
		 * @note	Internal method.
		 */
		void _registerFactory(RendererFactoryPtr factory);
	private:
		Vector<RendererFactoryPtr> mAvailableFactories;

		RendererPtr mActiveRenderer;
	};
}

