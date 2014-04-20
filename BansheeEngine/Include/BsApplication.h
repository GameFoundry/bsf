#pragma once

#include "BsPrerequisites.h"

#include "boost/signals/connection.hpp"

namespace BansheeEngine
{
	class BS_EXPORT BsApplication
	{
	public:
			BsApplication();

			/**
			 * @brief	Starts the application using the specified options. 
			 * 			This is how you start the engine.
			 */
			void startUp(RENDER_WINDOW_DESC& primaryWindowDesc, const String& renderSystem, const String& renderer);

			/**
			 * @brief	Executes the main loop. This will cause actually rendering to be performed
			 * 			and simulation to be run. Usually called immediately after startUp().
			 */
			void runMainLoop();

			/**
			 * @brief	Frees up all resources allocated during startUp, and while the application was running.
			 */
			void shutDown();

			const ViewportPtr& getPrimaryViewport() const;
	private:
		boost::signals::connection updateCallbackConn;
		DynLib* mMonoPlugin;
		DynLib* mSBansheeEnginePlugin;

		void update();
	};

	BS_EXPORT BsApplication& gBansheeApp();
}