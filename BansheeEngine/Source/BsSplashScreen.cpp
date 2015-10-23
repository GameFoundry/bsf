#include "BsSplashScreen.h"
#include "BsBuiltinResources.h"
#include "BsTimer.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include "Win32/BsWin32Window.h"

namespace BansheeEngine
{
	struct SplashScreen::Pimpl
	{
		Win32Window* window = nullptr;
		Timer timer;
	};

	// Note: Never freed, but that's fine
	SplashScreen::Pimpl* SplashScreen::m = bs_new<Pimpl>();
	const UINT32 SplashScreen::SPLASH_SCREEN_DURATION_MS = 3000;

	void SplashScreen::show()
	{
		if (m->window != nullptr)
			return;

		WINDOW_DESC windowDesc;
		windowDesc.border = WindowBorder::None;
		windowDesc.width = 600;
		windowDesc.height = 662;
		windowDesc.left = -1;
		windowDesc.top = -1;
		windowDesc.title = "Banshee Splash";
		windowDesc.toolWindow = true;
		windowDesc.alphaBlending = true;

		PixelDataPtr splashPixelData = BuiltinResources::getSplashScreen();
		if (splashPixelData == nullptr)
			return;

		windowDesc.background = splashPixelData;

		m->window = bs_new<Win32Window>(windowDesc);
		m->timer.reset();
	}

	void SplashScreen::hide()
	{
		if (m->window == nullptr || m->timer.getMilliseconds() < SPLASH_SCREEN_DURATION_MS)
			return;

		bs_delete(m->window);
		m->window = nullptr;
	}
}

#else

static_assert("Missing SplashScreen implementation.");

#endif