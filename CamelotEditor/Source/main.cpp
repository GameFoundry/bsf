#include "CmQtEditor.h"
#include <string>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CamelotEditor w;
	w.show();
	return a.exec();
}
