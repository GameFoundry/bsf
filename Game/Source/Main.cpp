#include "BsApplication.h"
#include "BsCrashHandler.h"
#include "BsCoreThread.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include <windows.h>

using namespace BansheeEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	CrashHandler::startUp();

	__try
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.videoMode = VideoMode(200, 200); // TODO - Get desktop resolution
		renderWindowDesc.title = "Banshee Game"; // TODO - Get this from game settings
		renderWindowDesc.fullscreen = false; // TODO - Get this from game settings
		renderWindowDesc.hideUntilSwap = true;

		RenderAPIPlugin renderAPI = RenderAPIPlugin::DX11; // TODO - Get this from game settings

		Application::startUp(renderWindowDesc, renderAPI);

		// TODO - Apply icon

		// TODO - If on first run start in fullscreen (perhaps Build settings controlled) at desktop resolution
		const VideoModeInfo& videoModeInfo = RenderAPI::getVideoModeInfo();
		const VideoOutputInfo& primaryMonitorInfo = videoModeInfo.getOutputInfo(0);
		const VideoMode& selectedVideoMode = primaryMonitorInfo.getDesktopVideoMode();

		RenderWindowPtr window = gApplication().getPrimaryWindow();
		window->setFullscreen(gCoreAccessor(), selectedVideoMode);

		// TODO - Load main scene

		Application::instance().runMainLoop();
		Application::shutDown();
	}
	__except (gCrashHandler().reportCrash(GetExceptionInformation()))
	{
		PlatformUtility::terminate(true);
	}

	CrashHandler::shutDown();

	return 0;
}
#endif // End BS_PLATFORM