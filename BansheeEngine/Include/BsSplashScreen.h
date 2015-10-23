#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Displays a splash screen with Banshee Engine logo.
	 */
	class BS_EXPORT SplashScreen
	{
	public:
		/**
		 * @brief	Displays a splash screen with Banshee Engine logo.
		 */
		static void show();

		/**
		 * @brief	Hides the splash screen.
		 */
		static void hide();

	private:
		struct Pimpl;
		static Pimpl* m;

		static const UINT32 SPLASH_SCREEN_DURATION_MS;
	};
}