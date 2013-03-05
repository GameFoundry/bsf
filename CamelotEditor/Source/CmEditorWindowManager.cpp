#include "CmEditorWindowManager.h"
#include "CmEditorApplication.h"
#include "CmQtEditorWindow.h"
#include "CmEditorWindowFactory.h"
#include "CmWindowDockManager.h"
#include "CmEditorPrefs.h"
#include "CmException.h"
#include "CmQtEditor.h"
#include <boost/bind.hpp>

namespace CamelotEditor
{
	struct WindowLayoutNode
	{
		QtEditorWindow* window;
		WindowLayoutNode* child;
	};

	void EditorWindowManager::registerWindowFactory(EditorWindowFactory* factory)
	{
		assert(factory != nullptr);

		mFactories[factory->getWindowName()] = factory;
	}

	QtEditorWindow* EditorWindowManager::openWindow(const QString& name)
	{
		auto iterFind = mOpenWindows.find(name);
		if(iterFind != mOpenWindows.end())
			return nullptr; // Window already open

		EditorWindowFactory* factory = getFactory(name);
		QtEditorWindow* window = factory->create(gEditorApp().getMainWindow());

		window->onClosed.connect(boost::bind(&EditorWindowManager::windowClosed, this, _1));
		window->setAttribute(Qt::WA_DeleteOnClose, true);
		window->show();

		mOpenWindows[name] = window;

		return window;
	}

	boost::function<void()> EditorWindowManager::getOpenCallback(const QString& name)
	{
		return boost::bind(&EditorWindowManager::openWindow, this, name);
	}

	QtEditorWindow* EditorWindowManager::getOpenWindow(const QString& name) const
	{
		auto iterFind = mOpenWindows.find(name);
		if(iterFind == mOpenWindows.end())
			CM_EXCEPT(InvalidParametersException, "There is no open window with name " + name.toStdString() + ".");

		return iterFind->second;
	}

	void EditorWindowManager::restoreWindowsFromPrefs()
	{
		vector<WindowLayoutDesc>::type windowLayouts = gEditorPrefs().getWindowLayouts();
		
		WindowLayoutNode* rootDockNode = nullptr;
		QString parentName = gWindowDockManager().getRootDockNodeName();
		bool foundDockedWindow = true;
		while(foundDockedWindow)
		{
			foundDockedWindow = false;
			for(auto iter = windowLayouts.begin(); iter != windowLayouts.end(); ++iter)
			{
				if(iter->dockState != WDS_FLOATING && iter->dockParentName == parentName)
				{
					QtEditorWindow* window = openWindow(iter->name);
					window->restoreFromLayoutDesc(*iter);
					parentName = window->getName();

					foundDockedWindow = true;
					break;
				}
			}
		}

		// Open non-docked windows
		for(auto iter = windowLayouts.begin(); iter != windowLayouts.end(); ++iter)
		{
			if(iter->dockState == WDS_FLOATING)
			{
				QtEditorWindow* window = openWindow(iter->name);
				window->restoreFromLayoutDesc(*iter);
			}
		}
	}

	void EditorWindowManager::saveWindowsToPrefs()
	{
		vector<WindowLayoutDesc>::type windowLayouts;
		for(auto iter = mOpenWindows.begin(); iter != mOpenWindows.end(); ++iter)
		{
			windowLayouts.push_back(iter->second->getLayoutDesc());
		}

		gEditorPrefs().setWindowLayouts(windowLayouts);
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

	void EditorWindowManager::windowClosed(QtEditorWindow* window)
	{
		assert(window != nullptr);

		auto iterFind = mOpenWindows.find(window->getName());
		if(iterFind == mOpenWindows.end())
			CM_EXCEPT(InternalErrorException, "Trying to close a window " + window->getName().toStdString() + " that is not in the open window list.");

		assert(iterFind->second == window);

		mOpenWindows.erase(iterFind);
	}

	EditorWindowManager& gEditorWindowManager()
	{
		return EditorWindowManager::instance();
	}
}