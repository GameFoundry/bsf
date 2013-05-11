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
	}

	void GUIManager::update()
	{
#if CM_DEBUG_MODE
		// Checks if all referenced windows actually exist
		std::vector<RenderWindow*> activeWindows = RenderWindowManager::instance().getRenderWindows();
		for(auto& widget : mWidgets)
		{
			auto iterFind = std::find(begin(activeWindows), end(activeWindows), widget->getOwnerWindow());

			if(iterFind == activeWindows.end())
			{
				CM_EXCEPT(InternalErrorException, "GUI manager has a reference to a window that doesn't exist. \
												  Please detach all GUIWidgets from windows before destroying a window.");
			}
		}
#endif

		for(auto& widget : mWidgets)
		{
			const RenderWindow* window = widget->getOwnerWindow();

			if(!window->getHasFocus())
				continue;

			Int2 screenPos = Cursor::getWindowPosition(*window);
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