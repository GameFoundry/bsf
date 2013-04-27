#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT Application
	{
	public:
			Application();

			/**
			 * @brief	Starts the application using the specified options. 
			 * 			This is how you start the engine.
			 */
			void startUp(CM::RENDER_WINDOW_DESC& primaryWindowDesc, const CM::String& renderSystem, const CM::String& renderer, const CM::String& resourceCacheDir);

			/**
			 * @brief	Executes the main loop. This will cause actually rendering to be performed
			 * 			and simulation to be run. Usually called immediately after startUp().
			 */
			void runMainLoop();

			/**
			 * @brief	Frees up all resources allocated during startUp, and while the application was running.
			 */
			void shutDown();
	};

	BS_EXPORT Application& gBansheeApp();
}