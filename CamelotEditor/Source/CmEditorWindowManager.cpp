#include "CmEditorWindowManager.h"
#include "CmEditorApplication.h"
#include "CmQtEditorWindow.h"
#include "CmEditorWindowFactory.h"
#include "CmException.h"
#include "CmQtEditor.h"
#include <boost/bind.hpp>

namespace CamelotEditor
{
	void EditorWindowManager::registerWindowFactory(EditorWindowFactory* factory)
	{
		assert(factory != nullptr);

		mFactories[factory->getWindowName()] = factory;
	}

	void EditorWindowManager::openWindow(const QString& name)
	{
		EditorWindowFactory* factory = getFactory(name);
		QtEditorWindow* window = factory->create(gEditorApp().getMainWindow());
		window->setAttribute(Qt::WA_DeleteOnClose, true);

		window->show();
	}

	boost::function<void()> EditorWindowManager::getOpenCallback(const QString& name)
	{
		return boost::bind(&EditorWindowManager::openWindow, this, name);
	}

	vector<QString>::type EditorWindowManager::getAvailableWindowTypes() const
	{
		vector<QString>::type types;
		for(auto iter = mFactories.begin(); iter != mFactories.end(); ++iter)
		{
			types.push_back(iter->first);
		}

		return types;
	}

	EditorWindowFactory* EditorWindowManager::getFactory(const QString& name) const
	{
		auto iterFind = mFactories.find(name);

		if(iterFind == mFactories.end())
			CM_EXCEPT(InvalidParametersException, "Window with the name: \"" + name.toStdString() + "\" doesn't exist.");

		return iterFind->second;
	}

	EditorWindowManager& gEditorWindowManager()
	{
		return EditorWindowManager::instance();
	}
}