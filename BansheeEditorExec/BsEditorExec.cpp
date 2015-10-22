#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "BsEditorApplication.h"
#include "BsPlatform.h"
#include "BsCrashHandler.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include <windows.h>
#include "Win32/BsWin32Window.h"

using namespace BansheeEngine;

#if BS_DEBUG_MODE
void InitializeDebugConsole()
{
	//Create a console for this application
	AllocConsole();
	//Redirect unbuffered STDOUT to the console
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
	FILE *COutputHandle = _fdopen(SystemOutput, "w");
	*stdout = *COutputHandle;
	setvbuf(stdout, NULL, _IONBF, 0);

	//Redirect unbuffered STDERR to the console
	HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
	int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
	FILE *CErrorHandle = _fdopen(SystemError, "w");
	*stderr = *CErrorHandle;
	setvbuf(stderr, NULL, _IONBF, 0);

	//Redirect unbuffered STDIN to the console
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
	FILE *CInputHandle = _fdopen(SystemInput, "r");
	*stdin = *CInputHandle;
	setvbuf(stdin, NULL, _IONBF, 0);

	//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
	std::ios::sync_with_stdio(true);
}

void ShutdownDebugConsole()
{
	//Write "Press any key to exit"
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD CharsWritten;
	WriteConsole(ConsoleOutput, "\nPress any key to exit", 22, &CharsWritten, 0);
	//Disable line-based input mode so we can get a single character
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(ConsoleInput, 0);
	//Read a single character
	TCHAR InputBuffer;
	DWORD CharsRead;
	ReadConsole(ConsoleInput, &InputBuffer, 1, &CharsRead, 0);
}
#endif // End BS_DEBUG_MODE

void ShowSplashScreen()
{
	//WINDOW_DESC windowDesc;
	//windowDesc.border = WindowBorder::None;
	//windowDesc.width = 600;
	//windowDesc.height = 662;
	//windowDesc.left = -1;
	//windowDesc.top = -1;
	//windowDesc.title = "Banshee Splash";
	//windowDesc.toolWindow = true;
	//windowDesc.alphaBlending = true;

	//Path splashTexturePath = "..\\..\\..\\Data\\Raw\\Engine\\BansheeLogo.png";

	//auto textureIO = std::static_pointer_cast<TextureImportOptions>(gImporter().createImportOptions(splashTexturePath));
	//textureIO->setCPUReadable(true);
	//HTexture splashTexture = gImporter().import<Texture>(splashTexturePath, textureIO);

	//PixelDataPtr splashPixelData = splashTexture->getProperties().allocateSubresourceBuffer(0);
	//splashTexture->readData(*splashPixelData);

	//windowDesc.background = splashPixelData;

	//bs_new<Win32Window>(windowDesc);
}

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
#if BS_DEBUG_MODE
	InitializeDebugConsole();
#endif

	CrashHandler::startUp();

	__try
	{
		EditorApplication::startUp(RenderAPIPlugin::DX11);
		EditorApplication::instance().runMainLoop();
		EditorApplication::shutDown();
	}
	__except (gCrashHandler().reportCrash(GetExceptionInformation()))
	{
		PlatformUtility::terminate(true);
	}

#if BS_DEBUG_MODE
	ShutdownDebugConsole();
#endif

	CrashHandler::shutDown();

	return 0;
}
#endif // End BS_PLATFORM