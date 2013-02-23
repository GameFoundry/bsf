#pragma once

#include "CmEditorPrerequisites.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class EditorApplication
	{
	public:
		void startUp();
		void run();
		void shutDown();

		void loadProject(const QString& projPath);
		void createProject(const QString& projPath);
		bool isValidProjectDirectory(const QString& projPath);
	};

	EditorApplication& gEditorApp();
}