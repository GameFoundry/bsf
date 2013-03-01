#include "CmEditorApplication.h"
#include "CmEditorPrefs.h"
#include "CmProjectPrefs.h"
#include "CmQtEditor.h"
#include "CmQtProjectSelection.h"
#include "CmEditorWindowManager.h"
#include "CmWindowDockManager.h"
#include "CmSceneWindowFactory.h"
#include "CmHierarchyWindowFactory.h"
#include "CmFileSystem.h"
#include "CmException.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDir>

namespace CamelotEditor
{
	const QString EditorApplication::PROJECT_PREFS_FILE_NAME = "CamelotProject.xml";
	const QString EditorApplication::EDITOR_PREFS_FILE_NAME = "Editor.xml";

	struct EditorApplication::PImpl
	{
		QApplication* mApp;
		QtEditor* mEditor;
	};

	EditorApplication::EditorApplication()
		:p(new PImpl())
	{
		p->mApp = nullptr;
		p->mEditor = nullptr;
	}

	EditorApplication::~EditorApplication()
	{
		delete p;
	}

	void EditorApplication::startUp()
	{
		EditorPrefs::startUp(new EditorPrefs());

		if(FileSystem::fileExists(getEditorPrefsPath().toStdString()))
			gEditorPrefs().load(getEditorPrefsPath());
		
		ProjectPrefs::startUp(new ProjectPrefs());

		int argc = 0;
		p->mApp = new QApplication(argc, nullptr);
		p->mEditor = new QtEditor();

		EditorWindowManager::startUp(new EditorWindowManager());
		WindowDockManager::startUp(new WindowDockManager(p->mEditor->getCentralWidget(), p->mEditor->getDockOverlayWidget()));

		gEditorWindowManager().registerWindowFactory(new SceneWindowFactory());
		gEditorWindowManager().registerWindowFactory(new HierarchyWindowFactory());

		vector<QString>::type windowTypes = gEditorWindowManager().getAvailableWindowTypes();
		for(auto iter = windowTypes.begin(); iter != windowTypes.end(); ++iter)
		{
			p->mEditor->addMenuItemCallback("Windows", *iter, gEditorWindowManager().getOpenCallback(*iter));
		}
	}

	void EditorApplication::run()
	{
		QtProjectSelection projSelection;
		projSelection.onProjectSelected.connect(boost::bind(&EditorApplication::loadProject, this, _1));
		
		if(projSelection.exec() == QDialog::Rejected)
			return;

		p->mEditor->setProjectName(gProjectPrefs().getProjectName());
		p->mEditor->show();
		p->mApp->exec();
	}

	void EditorApplication::shutDown()
	{
		WindowDockManager::shutDown();
		EditorWindowManager::shutDown();

		delete p->mApp;

		ProjectPrefs::shutDown();

		gEditorPrefs().save(getEditorPrefsPath());
		EditorPrefs::shutDown();
	}

	void EditorApplication::loadProject(const QString& absProjPath)
	{
		if(!isValidProject(absProjPath))
			CM_EXCEPT(InternalErrorException, "Cannot load project at path: " + absProjPath.toStdString());

		QString projectPrefsPath = getProjectPrefsPath(absProjPath);
		gProjectPrefs().load(projectPrefsPath);
	}

	void EditorApplication::createProject(const QString& absProjDir, const QString& projName)
	{
		QString fullProjPath = getFullProjectPath(absProjDir, projName);
		String fullProjPathStd = fullProjPath.toStdString();

		if(FileSystem::dirExists(fullProjPathStd))
			CM_EXCEPT(InternalErrorException, "Directory you have choosen for your project already exists: " + fullProjPathStd);

		FileSystem::createDir(fullProjPathStd);

		QString projPrefsPath = getProjectPrefsPath(fullProjPath);
		gProjectPrefs().setProjectName(projName);
		gProjectPrefs().save(projPrefsPath);
	}

	void EditorApplication::deleteProject(const QString& absProjPath)
	{
		FileSystem::deleteDir(absProjPath.toStdString());
	}

	bool EditorApplication::isValidProject(const QString& absProjDir) const
	{
		return projectExists(absProjDir);
	}

	bool EditorApplication::isValidProjectName(const QString& name) const
	{
		return FileSystem::isValidFileName(name.toStdString());
	}

	QString EditorApplication::getFullProjectPath(const QString& absProjDir, const QString& projName) const
	{
		QString dir = absProjDir;
		
		return QDir::cleanPath(QDir::toNativeSeparators(absProjDir + QDir::separator() + projName));
	}

	bool EditorApplication::projectExists(const QString& absProjPath) const
	{
		QString filePath = getProjectPrefsPath(absProjPath);

		if(FileSystem::fileExists(filePath.toStdString()))
			return true;

		return false;
	}

	QString EditorApplication::getProjectPrefsPath(const QString& absProjPath) const
	{
		return QDir::cleanPath(QDir::toNativeSeparators(absProjPath + QDir::separator() + PROJECT_PREFS_FILE_NAME));
	}

	QString EditorApplication::getEditorRootPath() const
	{
		return QString::fromStdString(FileSystem::getCurrentPath());
	}

	QString EditorApplication::getEditorPrefsPath() const
	{
		return QDir::cleanPath(QDir::toNativeSeparators(getEditorRootPath() + QDir::separator() + EDITOR_PREFS_FILE_NAME));
	}

	QtEditor* EditorApplication::getMainWindow() const
	{
		return p->mEditor;
	}

	EditorApplication& gEditorApp()
	{
		static EditorApplication application;
		return application;
	}
}