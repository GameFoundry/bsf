#include "CmEditorWindowManager.h"
#include "CmEditorApplication.h"
#include "CmQtEditorWindow.h"
#include "CmEditorWidgetFactory.h"
#include "CmWindowDockManager.h"
#include "CmEditorPrefs.h"
#include "CmException.h"
#include "CmQtEditor.h"
#include <boost/bind.hpp>

namespace CamelotEditor
{
	EditorWindowManager::EditorWindowManager()
		:mMaxOpenWindowId(0)
	{ }

	void EditorWindowManager::registerWidgetFactory(EditorWidgetFactory* factory)
	{
		assert(factory != nullptr);

		mFactories[factory->getWindowName()] = factory;
	}

	void EditorWindowManager::openWidget(const QString& name, QtEditorWindow* parent)
	{
		auto iterFind = mOpenWidgets.find(name);
		if(iterFind != mOpenWidgets.end())
			return; // Widget already open

		EditorWidgetFactory* factory = getFactory(name);
		
		if(parent == nullptr)
			parent = openWindow();
		
		QtEditorWidget* widget = factory->create(parent);
		parent->addWidget(widget);

		mOpenWidgets[name] = widget;
		widget->onClosed.connect(boost::bind(&EditorWindowManager::widgetClosed, this, _1));
	}

	boost::function<void()> EditorWindowManager::getOpenCallback(const QString& name)
	{
		return boost::bind(&EditorWindowManager::openWidget, this, name, nullptr);
	}

	QtEditorWindow* EditorWindowManager::openWindow(INT32 forcedId)
	{
		if(forcedId != -1)
		{
			auto iterFindId = mOpenWindows.find(forcedId);
			if(iterFindId != mOpenWindows.end())
				CM_EXCEPT(InvalidParametersException, "Window with the specified id already exists: " + toString(forcedId));
		}

		INT32 windowId = 0;
		if(forcedId != -1)
			windowId = forcedId;
		else
			windowId = mMaxOpenWindowId;

		mMaxOpenWindowId = windowId + 1;

		QtEditorWindow* window = new QtEditorWindow(gEditorApp().getMainWindow(), windowId);
		mOpenWindows[windowId] = window;

		window->onClosed.connect(boost::bind(&EditorWindowManager::windowClosed, this, _1));
		window->setAttribute(Qt::WA_DeleteOnClose, true);
		window->show();

		return window;
	}

	QtEditorWindow* EditorWindowManager::getOpenWindow(INT32 id) const
	{
		auto iterFind = mOpenWindows.find(id);
		if(iterFind == mOpenWindows.end())
			CM_EXCEPT(InvalidParametersException, "There is no open window with id " + toString(id) + ".");

		return iterFind->second;
	}

	QtEditorWindow* EditorWindowManager::getWindowAtPosition(const QPoint& globalPos, Vector<UINT32>::type windowsToIgnore) const
	{
		for(auto iter = mOpenWindows.begin(); iter != mOpenWindows.end(); ++iter)
		{
			QtEditorWindow* curWindow = iter->second;

			auto iterFind = std::find(windowsToIgnore.begin(), windowsToIgnore.end(), curWindow->getId());
			if(iterFind != windowsToIgnore.end())
				continue;

			QPoint globalWidgetPos = curWindow->mapToGlobal(QPoint(0, 0));
			QRect widgetRect(globalWidgetPos, curWindow->geometry().size());

			if(widgetRect.contains(globalPos))
				return curWindow;
		}

		return nullptr;
	}

	void EditorWindowManager::restoreWindowsFromPrefs()
	{
		Vector<WindowLayoutDesc>::type windowLayouts = gEditorPrefs().getWindowLayouts();
		
		UINT32 parentId = -1;
		bool foundDockedWindow = true;
		while(foundDockedWindow)
		{
			foundDockedWindow = false;
			for(auto iter = windowLayouts.begin(); iter != windowLayouts.end(); ++iter)
			{
				if(iter->dockState != WDS_FLOATING && iter->dockParentId == parentId)
				{
					QtEditorWindow* window = openWindow(iter->id);
					
					window->restoreFromLayoutDesc(*iter);
					parentId = window->getId();

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
				QtEditorWindow* window = openWindow(iter->id);
				window->restoreFromLayoutDesc(*iter);
			}
		}
	}

	void EditorWindowManager::saveWindowsToPrefs()
	{
		Vector<WindowLayoutDesc>::type windowLayouts;
		for(auto iter = mOpenWindows.begin(); iter != mOpenWindows.end(); ++iter)
		{
			windowLayouts.push_back(iter->second->getLayoutDesc());
		}

		gEditorPrefs().setWindowLayouts(windowLayouts);
	}

	Vector<QString>::type EditorWindowManager::getAvailableWindowTypes() const
	{
		Vector<QString>::type types;
		for(auto iter = mFactories.begin(); iter != mFactories.end(); ++iter)
		{
			types.push_back(iter->first);
		}

		return types;
	}

	EditorWidgetFactory* EditorWindowManager::getFactory(const QString& name) const
	{
		auto iterFind = mFactories.find(name);

		if(iterFind == mFactories.end())
			CM_EXCEPT(InvalidParametersException, "Window with the name: \"" + name.toStdString() + "\" doesn't exist.");

		return iterFind->second;
	}

	void EditorWindowManager::widgetClosed(QtEditorWidget* widget)
	{
		assert(widget != nullptr);

		auto iterFind = mOpenWidgets.find(widget->getName());
		if(iterFind == mOpenWidgets.end())
			CM_EXCEPT(InternalErrorException, "Trying to close a widget " + widget->getName().toStdString() + " that is not in the open widget list.");

		assert(iterFind->second == widget);

		mOpenWidgets.erase(iterFind);
	}

	void EditorWindowManager::windowClosed(QtEditorWindow* window)
	{
		assert(window != nullptr);

		auto iterFind = mOpenWindows.find(window->getId());
		if(iterFind == mOpenWindows.end())
			CM_EXCEPT(InternalErrorException, "Trying to close a window " + toString(window->getId()) + " that is not in the open window list.");

		assert(iterFind->second == window);

		mOpenWindows.erase(iterFind);
	}

	EditorWindowManager& gEditorWindowManager()
	{
		return EditorWindowManager::instance();
	}
}