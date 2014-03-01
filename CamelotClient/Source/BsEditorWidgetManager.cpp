#include "BsEditorWidgetManager.h"
#include "BsEditorWidget.h"
#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"
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

		auto iterFindCreate = mCreateCallbacks.find(name);
		if(iterFindCreate == mCreateCallbacks.end())
			CM_EXCEPT(InvalidParametersException, "Trying to open a widget that is not registered with the widget manager. Name: \"" + name + "\"");

		EditorWidgetBase* newWidget = mCreateCallbacks[name]();
		EditorWindow* window = EditorWindow::create();
		window->widgets().add(*newWidget);
		newWidget->initialize();

		mActiveWidgets[name] = newWidget;

		return newWidget;
	}

	void EditorWidgetManager::close(EditorWidgetBase* widget)
	{
		auto findIter = std::find_if(mActiveWidgets.begin(), mActiveWidgets.end(),
			[&] (const std::pair<String, EditorWidgetBase*>& entry) { return entry.second == widget; });

		if(findIter != mActiveWidgets.end())
			mActiveWidgets.erase(findIter);

		widget->mParent->_notifyWidgetDestroyed(widget);

		EditorWidgetBase::destroy(widget);
	}

	void EditorWidgetManager::preRegisterWidget(const String& name, std::function<EditorWidgetBase*()> createCallback)
	{
		QueuedCreateCallbacks.push(std::pair<String, std::function<EditorWidgetBase*()>>(name, createCallback));
	}
}