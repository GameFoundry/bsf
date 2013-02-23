#include "CmEditorApplication.h"
#include "CmLayoutManager.h"
#include "CmEditorPrefs.h"
#include "CmQtEditor.h"
#include "CmQtProjectSelection.h"
#include <QtWidgets/QApplication>

using namespace CamelotEngine;

namespace CamelotEditor
{
	void EditorApplication::startUp()
	{
		EditorPrefs::startUp(new EditorPrefs());
		LayoutManager::startUp(new LayoutManager());
	}

	void EditorApplication::run()
	{
		int argc = 0;
		QApplication a(argc, nullptr);
		QtEditor w;
		w.show();
		a.exec();
	}

	void EditorApplication::shutDown()
	{
		LayoutManager::shutDown();
		EditorPrefs::shutDown();
	}
}