#include "BsEditorWidgetManager.h"
#include "BsEditorWidget.h"
#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWindowManager.h"
#include "BsMainEditorWindow.h"
#include "BsEditorWidgetLayout.h"
#include "BsDockManager.h"
#include "CmException.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	Stack<std::pair<String, std::function<EditorWidgetBase*()>>>::type EditorWidgetManager::QueuedCreateCallbacks;

	EditorWidgetManager::EditorWidgetManager()
	{
		while(!QueuedCreateCallbacks.empty())
		{
			std::pair<String, std::function<EditorWidgetBase*()>> curElement = QueuedCreateCallbacks.top();
			QueuedCreateCallbacks.pop();

			registerWidget(curElement.first, curElement.second);
		}
	}

	void EditorWidgetManager::registerWidget(const String& name, std::function<EditorWidgetBase*()> createCallback)
	{
		auto iterFind = mCreateCallbacks.find(name);

		if(iterFind != mCreateCallbacks.end())
			CM_EXCEPT(InvalidParametersException, "Widget with the same name is already registered. Name: \"" + name + "\"");

		mCreateCallbacks[name] = createCallback;
	}

	EditorWidgetBase* EditorWidgetManager::open(const String& name)
	{
		auto iterFind = mActiveWidgets.find(name);

		if(iterFind != mActiveWidgets.end())
			return iterFind->second;

		EditorWidgetBase* newWidget = create(name);
		if(newWidget == nullptr)
			return nullptr;

		EditorWindow* window = EditorWindow::create();
		window->widgets().add(*newWidget);
		newWidget->initialize();

		return newWidget;
	}

	void EditorWidgetManager::close(EditorWidgetBase* widget)
	{
		auto findIter = std::find_if(mActiveWidgets.begin(), mActiveWidgets.end(),
			[&] (const std::pair<String, EditorWidgetBase*>& entry) { return entry.second == widget; });

		if(findIter != mActiveWidgets.end())
			mActiveWidgets.erase(findIter);

		if(widget->mParent != nullptr)
			widget->mParent->_notifyWidgetDestroyed(widget);

		EditorWidgetBase::destroy(widget);
	}

	bool EditorWidgetManager::isOpen(const CM::String& name) const
	{
		auto iterFind = mActiveWidgets.find(name);

		return iterFind != mActiveWidgets.end();
	}

	EditorWidgetBase* EditorWidgetManager::create(const CM::String& name)
	{
		auto iterFind = mActiveWidgets.find(name);

		if(iterFind != mActiveWidgets.end())
			return iterFind->second;

		auto iterFindCreate = mCreateCallbacks.find(name);
		if(iterFindCreate == mCreateCallbacks.end())
			return nullptr;

		EditorWidgetBase* newWidget = mCreateCallbacks[name]();

		if(newWidget != nullptr)
			mActiveWidgets[name] = newWidget;

		return newWidget;
	}

	EditorWidgetLayoutPtr EditorWidgetManager::getLayout() const
	{
		auto GetWidgetNamesInContainer = [&] (const EditorWidgetContainer* container)
		{
			Vector<String>::type widgetNames;
			if(container != nullptr)
			{
				UINT32 numWidgets = container->getNumWidgets();

				for(UINT32 i = 0; i < numWidgets; i++)
				{
					EditorWidgetBase* widget = container->getWidget(i);
					widgetNames.push_back(widget->getName());
				}				
			}

			return widgetNames;
		};

		MainEditorWindow* mainWindow = EditorWindowManager::instance().getMainWindow();
		DockManager& dockManager = mainWindow->getDockManager();
		EditorWidgetLayoutPtr layout = cm_shared_ptr<EditorWidgetLayout>(dockManager.getLayout());

		Vector<EditorWidgetLayout::Entry>::type& layoutEntries = layout->getEntries();
		UnorderedSet<EditorWidgetContainer*>::type widgetContainers;

		for(auto& widget : mActiveWidgets)
		{
			widgetContainers.insert(widget.second->_getParent());
		}

		for(auto& widgetContainer : widgetContainers)
		{
			if(widgetContainer == nullptr)
				continue;

			layoutEntries.push_back(EditorWidgetLayout::Entry());
			EditorWidgetLayout::Entry& entry = layoutEntries.back();

			entry.widgetNames = GetWidgetNamesInContainer(widgetContainer);

			EditorWindow* parentWindow = widgetContainer->getParentWindow();
			entry.isDocked = parentWindow == nullptr;
			
			if(!entry.isDocked)
			{
				entry.x = parentWindow->getLeft();
				entry.y = parentWindow->getTop();
				entry.width = parentWindow->getWidth();
				entry.height = parentWindow->getHeight();
			}
		}

		return layout;
	}

	void EditorWidgetManager::setLayout(const EditorWidgetLayoutPtr& layout)
	{
		Vector<EditorWidgetBase*>::type openWidgets;
		Vector<EditorWidgetBase*>::type widgetsNeedInitialization;

		// Create any necessary widgets
		for(auto& entry : layout->getEntries())
		{
			for(auto& widgetName : entry.widgetNames)
			{
				bool needsInitialization = !isOpen(widgetName);
				EditorWidgetBase* newWidget = create(widgetName);
				if(newWidget != nullptr)
				{
					openWidgets.push_back(newWidget);

					if(needsInitialization)
						widgetsNeedInitialization.push_back(newWidget);
				}
			}
		}

		// Unparent all widgets
		Vector<EditorWidgetBase*>::type unparentedWidgets;
		for(auto& widget : mActiveWidgets)
		{
			if(widget.second->_getParent() != nullptr)
				widget.second->_getParent()->remove(*(widget.second));

			unparentedWidgets.push_back(widget.second);
		}

		// Restore floating widgets
		for(auto& entry : layout->getEntries())
		{
			if(entry.isDocked)
				continue;

			EditorWindow* window = EditorWindow::create();
			for(auto& widgetName : entry.widgetNames)
			{
				EditorWidgetBase* widget = create(widgetName); // This will returned previously created widget
				if(widget != nullptr)
					window->widgets().add(*widget);
			}

			window->setPosition(entry.x, entry.y);
			window->setSize(entry.width, entry.height);
		}

		// Restore docked widgets
		MainEditorWindow* mainWindow = EditorWindowManager::instance().getMainWindow();
		DockManager& dockManager = mainWindow->getDockManager();

		dockManager.setLayout(layout->getDockLayout(), openWidgets);

		// Initialize any newly opened widgets
		for(auto& widget : widgetsNeedInitialization)
		{
			if(widget->_getParent() != nullptr)
				widget->initialize();
		}

		// Destroy any widgets that are no longer have parents
		for(auto& widget : unparentedWidgets)
		{
			if(widget->_getParent() == nullptr)
				widget->close();
		}
	}

	void EditorWidgetManager::preRegisterWidget(const String& name, std::function<EditorWidgetBase*()> createCallback)
	{
		QueuedCreateCallbacks.push(std::pair<String, std::function<EditorWidgetBase*()>>(name, createCallback));
	}
}