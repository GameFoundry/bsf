#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "BsGUIMouseEvent.h"
#include "CmSceneObject.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"
#include "CmRenderWindowManager.h"
#include "CmCursor.h"
#include "CmException.h"
#include "CmInput.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIManager::GUIManager()
		:mMouseOverElement(nullptr), mMouseOverWidget(nullptr)
	{
		for(int i = 0; i < MB_Count; i++)
			mLastFrameButtonState[i] = false;
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

		if(mMouseOverWidget == widget)
		{
			mMouseOverWidget = nullptr;
			mMouseOverElement = nullptr;
		}
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
		GUIWidget* widgetInFocus = nullptr;
		GUIElement* topMostElement = nullptr;
		UINT32 topMostDepth = std::numeric_limits<UINT32>::max();

		for(auto& widget : mWidgets)
		{
			const RenderWindow* window = widget->getOwnerWindow();

			if(window->hasFocus())
			{
				widgetInFocus = widget;
				break;
			}
		}

		Int2 screenPos;
		if(widgetInFocus != nullptr)
		{
			const RenderWindow* window = widgetInFocus->getOwnerWindow();

			screenPos = Cursor::getWindowPosition(*window);
			Vector3 vecScreenPos((float)screenPos.x, (float)screenPos.y, 0.0f);

			GUIElement* topMostElement = nullptr;
			INT32 topMostDepth = std::numeric_limits<INT32>::max();
			for(auto& widget : mWidgets)
			{
				if(widget->inBounds(screenPos))
				{
					const Matrix4& worldTfrm = widget->SO()->getWorldTfrm();

					Vector3 vecLocalPos = worldTfrm.inverse() * vecScreenPos;
					Int2 localPos(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));

					const std::vector<GUIElement*>& elements = widget->getElements();

					// Elements with lowest depth (most to the front) get handled first
					for(auto iter = elements.rbegin(); iter != elements.rend(); ++iter)
					{
						GUIElement* element = *iter;
						const Rect& bounds = element->getBounds();

						if(bounds.contains(localPos) && element->getDepth() < topMostDepth)
						{
							topMostElement = element;
							topMostDepth = element->getDepth();

							break;
						}
					}
				}
			}
		}

		if(topMostElement != mMouseOverElement)
		{
			if(mMouseOverElement != nullptr)
			{
				// Send MouseOut event
				const RenderWindow* window = mMouseOverWidget->getOwnerWindow();
				Int2 oldScreenPos = Cursor::getWindowPosition(*window);

				GUIMouseEvent event(GUIMouseEventType::MouseOut, oldScreenPos); // TODO - This pos will be wrong as it might be related to completely other window
				mMouseOverElement->mouseEvent(event);
			}

			if(topMostElement != nullptr)
			{
				// Send MouseOver event
				GUIMouseEvent event(GUIMouseEventType::MouseOver, screenPos);
				topMostElement->mouseEvent(event);
			}

			mMouseOverElement = topMostElement;
			mMouseOverWidget = widgetInFocus;
		}

		if(mMouseOverElement != nullptr)
		{
			// Send MouseMove event
			if(mLastCursorPos != screenPos)
			{
				GUIMouseEvent event(GUIMouseEventType::MouseMove, screenPos);
				mMouseOverElement->mouseEvent(event);
			}

			// Send MouseDown and MouseUp events
			for(int i = 0; i < MB_Count; i++)
			{
				bool buttonDown = gInput().isButtonDown((MouseButton)i);
				if(mLastFrameButtonState[i] != buttonDown)
				{
					if(buttonDown)
					{
						GUIMouseEvent event(GUIMouseEventType::MouseDown, screenPos, (MouseButton)i);
						mMouseOverElement->mouseEvent(event);
					}
					else
					{
						GUIMouseEvent event(GUIMouseEventType::MouseUp, screenPos, (MouseButton)i);
						mMouseOverElement->mouseEvent(event);
					}

					mLastFrameButtonState[i] = buttonDown;
				}
			}
		}
	}
}