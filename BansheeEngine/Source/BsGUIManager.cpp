#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "BsGUIMouseEvent.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"
#include "CmRenderWindowManager.h"
#include "CmCursor.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIManager::GUIManager()
	{

	}

	GUIManager::~GUIManager()
	{

	}

	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(widget);
	}

	void GUIManager::unregisterWidget(GUIWidget* widget)
	{
		auto findIter = std::find(begin(mWidgets), end(mWidgets), widget);

		if(findIter != end(mWidgets))
			mWidgets.erase(findIter);

		for(auto& windowMap : mWindowWidgetMap)
		{
			auto& widgets = windowMap.second;
			auto iterFind = std::find(begin(widgets), end(widgets), widget);

			if(iterFind != end(widgets))
				widgets.erase(iterFind);
		}
	}

	void GUIManager::attachWidgetToWindow(const RenderWindow* window, GUIWidget* widget)
	{
		auto findIter = mWindowWidgetMap.find(window);

		if(findIter == mWindowWidgetMap.end())
		{
			mWindowWidgetMap.insert(std::make_pair(window, std::vector<GUIWidget*>()));
			findIter = mWindowWidgetMap.find(window);
		}

		std::vector<GUIWidget*>& widgets = findIter->second;

		bool found = false;
		for(auto& existingWidget : widgets)
		{
			if(existingWidget == widget)
			{
				found = true;
				break;
			}
		}

		if(!found)
			widgets.push_back(widget);
	}

	void GUIManager::detachWidgetFromWindow(const CM::RenderWindow* window, GUIWidget* widget)
	{
		auto findIter = mWindowWidgetMap.find(window);

		if(findIter == mWindowWidgetMap.end())
		{
			CM_EXCEPT(InternalErrorException, "Cannot find window to detach the widget from.");
		}

		std::vector<GUIWidget*>& widgets = findIter->second;
		auto findIter2 = std::find(begin(widgets), end(widgets), widget);

		if(findIter2 == widgets.end())
		{
			CM_EXCEPT(InternalErrorException, "Cannot find widget attached to the specified window.");
		}

		widgets.erase(findIter2);
	}

	void GUIManager::update()
	{
#if CM_DEBUG_MODE
		// Checks if all referenced windows actually exist
		std::vector<RenderWindow*> activeWindows = RenderWindowManager::instance().getRenderWindows();
		for(auto& window : mWindowWidgetMap)
		{
			auto iterFind = std::find(begin(activeWindows), end(activeWindows), window.first);

			if(iterFind == activeWindows.end())
			{
				CM_EXCEPT(InternalErrorException, "GUI manager has a reference to a window that doesn't exist. \
												  Please detach all GUIWidgets from windows before destroying a window.");
			}
		}
#endif

		for(auto& window : mWindowWidgetMap)
		{
			if(!window.first->getHasFocus())
				continue;

			Int2 screenPos = Cursor::getWindowPosition(*window.first);
			GUIMouseEvent mouseEvent(screenPos);

			for(auto& widget : mWidgets)
			{
				if(widget->inBounds(screenPos))
				{
					if(widget->mouseEvent(mouseEvent))
						break;
				}
			}
		}
	}
}