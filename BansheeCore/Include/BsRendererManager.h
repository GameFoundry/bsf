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
		~RendererManager();

		/**
		 * @brief	Attempts to find a renderer with the specified name and makes it active.
		 * 			Exception is thrown if renderer with the specified name doesn't exist.
		 * 			You must call ::initialize after setting the active renderer to properly activate it.
		 */
		void setActive(const String& name);

		/**
		 * @brief	Initializes the currently active renderer, making it ready to render.
		 */
		void initialize();

		/**
		 * @brief	Returns the currently active renderer. Null if no renderer is active.
		 */
		CoreRendererPtr getActive() { return mActiveRenderer; }

		/**
		 * @brief	Registers a new renderer factory. Any renderer you try to make active with
		 * 			"setActive" you will need to have previously registered here.
		 *
		 * @note	Internal method.
		 */
		void _registerFactory(RendererFactoryPtr factory);
	private:
		Vector<RendererFactoryPtr> mAvailableFactories;

		CoreRendererPtr mActiveRenderer;
	};
}

