#include "CmQtEditor.h"
#include <string>
#include <QtWidgets/QApplication>
#include <windows.h>

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	int argc = 1;
	char** argv = new char*[1];
	argv[0] = "CamelotEditor.exe";

	QApplication a(argc, argv);
	CamelotEditor w;
	w.show();
	return a.exec();
}

