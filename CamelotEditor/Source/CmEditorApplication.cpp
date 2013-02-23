#include "CmEditorApplication.h"
#include "CmLayoutManager.h"
#include "CmEditorPrefs.h"
#include "CmQtEditor.h"
#include "CmQtProjectSelection.h"
#include <QtWidgets/QApplication>

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

		QtProjectSelection projSelection;
		projSelection.onProjectSelected.connect(boost::bind(&EditorApplication::loadProject, this, _1));
		projSelection.exec();

		a.exec();
	}

	void EditorApplication::shutDown()
	{
		LayoutManager::shutDown();
		EditorPrefs::shutDown();
	}

	void EditorApplication::loadProject(const QString& projPath)
	{
		// TODO
	}

	void EditorApplication::createProject(const QString& projPath)
	{
		// TODO
	}

	bool EditorApplication::isValidProjectDirectory(const QString& projPath)
	{
		// TODO
		return true;
	}

	EditorApplication& gEditorApp()
	{
		static EditorApplication application;
		return application;
	}
}