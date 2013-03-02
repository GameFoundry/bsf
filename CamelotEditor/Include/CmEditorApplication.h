#pragma once

#include "CmEditorPrerequisites.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class EditorApplication
	{
		struct PImpl;

	public:
		EditorApplication();
		~EditorApplication();

		void startUp();
		void run();
		void shutDown();

		void loadProject(const QString& absProjPath);
		void createProject(const QString& absProjDir, const QString& projName);
		void deleteProject(const QString& absProjPath);

		bool isValidProject(const QString& absProjDir) const;
		bool isValidProjectName(const QString& name) const;
		QString getFullProjectPath(const QString& absProjDir, const QString& projName) const;
		bool projectExists(const QString& absProjPath) const;

		QString getEditorRootPath() const;

		QtEditor* getMainWindow() const;
		
	private:
		static const QString PROJECT_PREFS_FILE_NAME;
		static const QString EDITOR_PREFS_FILE_NAME;
		static const QString STYLE_SHEETS_DIRECTORY_NAME;

		PImpl* p;

		QString getProjectPrefsPath(const QString& absProjPath) const;
		QString getEditorPrefsPath() const;
		QString getStyleSheetsDirectoryPath() const;

		void startUpQt();
		void shutDownQt();
		void loadStyleSheets();
	};

	EditorApplication& gEditorApp();
}