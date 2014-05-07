#include "stdafx.h"
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
	EditorApplication::startUp(cm_new<EditorApplication>(RenderSystemPlugin::OpenGL));
	EditorApplication::instance().runMainLoop();
	EditorApplication::shutDown();

	return 0;
}