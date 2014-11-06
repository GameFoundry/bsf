#include <windows.h>

#include "BsEditorApplication.h"

using namespace BansheeEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	EditorApplication::startUp(RenderSystemPlugin::DX9);
	EditorApplication::instance().runMainLoop();
	EditorApplication::shutDown();

	return 0;
}