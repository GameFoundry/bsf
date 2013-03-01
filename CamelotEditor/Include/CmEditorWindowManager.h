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

		void openWindow(const QString& name);
		boost::function<void()> getOpenCallback(const QString& name);

		vector<QString>::type getAvailableWindowTypes() const;

	private:
		map<QString, EditorWindowFactory*>::type mFactories;

		EditorWindowFactory* getFactory(const QString& name) const;
	};

	EditorWindowManager& gEditorWindowManager();
}