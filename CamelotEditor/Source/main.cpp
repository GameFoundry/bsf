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
	EditorApplication app;
	app.startUp();

	app.run();

	app.shutDown();
}
