#include "CmEditorApplication.h"
#include "CmEditorPrefs.h"
#include "CmProjectPrefs.h"
#include "CmQtEditor.h"
#include "CmQtProjectSelection.h"
#include "CmEditorWindowManager.h"
#include "CmWindowDockManager.h"
#include "CmSceneWidgetFactory.h"
#include "CmHierarchyWidgetFactory.h"
#include "CmFileSystem.h"
#include "CmException.h"
#include "CmDataStream.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDir>

namespace CamelotEditor
{
	const QString EditorApplication::PROJECT_PREFS_FILE_NAME = "CamelotProject.xml";
	const QString EditorApplication::EDITOR_PREFS_FILE_NAME = "Editor.xml";
	const QString EditorApplication::STYLE_SHEETS_DIRECTORY_NAME = "Styles";

	struct EditorApplication::PImpl
	{
		QApplication* mApp;
		QtEditor* mEditor;
	};

	EditorApplication::EditorApplication()
		:p(CM_NEW(PImpl, GenAlloc) PImpl())
	{
		p->mApp = nullptr;
		p->mEditor = nullptr;
	}

	EditorApplication::~EditorApplication()
	{
		CM_DELETE(p, PImpl, GenAlloc);
	}

	void EditorApplication::startUp()
	{
		EditorPrefs::startUp(CM_NEW(EditorPrefs, GenAlloc) EditorPrefs());

		if(FileSystem::fileExists(getEditorPrefsPath().toStdString()))
			gEditorPrefs().load(getEditorPrefsPath());
		
		ProjectPrefs::startUp(CM_NEW(EditorPrefs, GenAlloc) ProjectPrefs());

		startUpQt();

		EditorWindowManager::startUp(CM_NEW(EditorWindowManager, GenAlloc) EditorWindowManager());
		WindowDockManager::startUp(CM_NEW(WindowDockManager, GenAlloc) WindowDockManager(p->mEditor->getCentralWidget(), p->mEditor->getDockOverlayWidget()));

		gEditorWindowManager().registerWidgetFactory(new SceneWidgetFactory());
		gEditorWindowManager().registerWidgetFactory(new HierarchyWidgetFactory());

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

		gEditorWindowManager().restoreWindowsFromPrefs();

		p->mEditor->setProjectName(gProjectPrefs().getProjectName());
		p->mEditor->show();
		p->mApp->exec();
	}

	void EditorApplication::shutDown()
	{
		gEditorWindowManager().saveWindowsToPrefs();

		WindowDockManager::shutDown();
		EditorWindowManager::shutDown();

		shutDownQt();
		
		ProjectPrefs::shutDown();

		gEditorPrefs().save(getEditorPrefsPath());
		EditorPrefs::shutDown();
	}

	void EditorApplication::startUpQt()
	{
		int argc = 0;
		p->mApp = new QApplication(argc, nullptr);
		p->mEditor = new QtEditor();

		loadStyleSheets();
	}

	void EditorApplication::shutDownQt()
	{
		delete p->mApp;
	}

	void EditorApplication::loadStyleSheets()
	{
		QDir styleSheetsDir = getStyleSheetsDirectoryPath();

		if(!styleSheetsDir.exists())
			return;

		QStringList nameFilters;
		nameFilters<<"*.css";

		QFileInfoList fileInfos = styleSheetsDir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);

		QString styleSheet;
		for(auto iter = fileInfos.begin(); iter != fileInfos.end(); ++iter)
		{
			DataStreamPtr dataStream = FileSystem::open(iter->absoluteFilePath().toStdString());

			styleSheet += QString::fromStdString(dataStream->getAsString());
		}

		p->mApp->setStyleSheet(styleSheet);
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

	QString EditorApplication::getStyleSheetsDirectoryPath() const
	{
		return QDir::cleanPath(QDir::toNativeSeparators(getEditorRootPath() + QDir::separator() + STYLE_SHEETS_DIRECTORY_NAME));
	}

	EditorApplication& gEditorApp()
	{
		static EditorApplication application;
		return application;
	}
}