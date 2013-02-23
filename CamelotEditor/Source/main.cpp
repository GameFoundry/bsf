#include <windows.h>
#include "CmEditorApplication.h"

#include "CmPreferences.h"

using namespace CamelotEditor;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	Preferences prefs;
	prefs.setValue("testA", "Haloo sta ima!?");
	prefs.setValue("testA", "Haloo sta ima 222!?", 1);
	prefs.addValue("testFlt", 10.534536346f);
	prefs.addValue("testFlt", 5.2423423423f);
	prefs.setValue("testInt", 10);
	prefs.setValue("SomeGroup.testInt1", 10);
	prefs.setValue("SomeGroup.testInt2", 11);
	prefs.setValue("SomeGroup.testInt3", 12);

	prefs.save("D:\\testPrefs.ini");

	prefs.load("D:\\testPrefs.ini");
	prefs.save("D:\\testPrefs2.ini");

	gEditorApp().startUp();

	gEditorApp().run();

	gEditorApp().shutDown();
}
