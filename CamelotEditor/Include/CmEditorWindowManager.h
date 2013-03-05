#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include <boost/function.hpp>
#include <QtCore/QString>

namespace CamelotEditor
{
	class EditorWindowManager : public Module<EditorWindowManager>
	{		
	public:
		void registerWindowFactory(EditorWindowFactory* factory);

		QtEditorWindow* openWindow(const QString& name);
		boost::function<void()> getOpenCallback(const QString& name);

		QtEditorWindow* getOpenWindow(const QString& name) const;

		void restoreWindowsFromPrefs();
		void saveWindowsToPrefs();

		vector<QString>::type getAvailableWindowTypes() const;

	private:
		map<QString, EditorWindowFactory*>::type mFactories;
		map<QString, QtEditorWindow*>::type mOpenWindows;

		EditorWindowFactory* getFactory(const QString& name) const;

		void windowClosed(QtEditorWindow* window);
	};

	EditorWindowManager& gEditorWindowManager();
}