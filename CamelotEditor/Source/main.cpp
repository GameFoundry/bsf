#include <windows.h>
#include "CmEditorApplication.h"

using namespace CamelotEditor;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	gEditorApp().startUp();

	gEditorApp().run();

	gEditorApp().shutDown();
}
