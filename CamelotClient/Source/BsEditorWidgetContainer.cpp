#include "BsEditorWidgetContainer.h"
#include "BsGUITabbedTitleBar.h"
#include "BsEditorWidget.h"
#include "BsDragAndDropManager.h"
#include "BsEditorWindow.h"
#include "CmMath.h"
#include "CmInput.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 EditorWidgetContainer::TitleBarHeight = 13;

	EditorWidgetContainer::EditorWidgetContainer(BS::GUIWidget* parent, RenderWindow* renderWindow)
		:mParent(parent), mX(0), mY(0), mWidth(0), mHeight(0), mTitleBar(nullptr), mActiveWidget(-1)
	{
		mTitleBar = cm_new<GUITabbedTitleBar>(parent, renderWindow);
		mTitleBar->onTabActivated.connect(boost::bind(&EditorWidgetContainer::tabActivated, this, _1));
		mTitleBar->onTabClosed.connect(boost::bind(&EditorWidgetContainer::tabClosed, this, _1));
		mTitleBar->onTabDraggedOff.connect(boost::bind(&EditorWidgetContainer::tabDraggedOff, this, _1));
		mTitleBar->onTabDraggedOn.connect(boost::bind(&EditorWidgetContainer::tabDraggedOn, this, _1));
	}

	EditorWidgetContainer::~EditorWidgetContainer()
	{
		cm_delete(mTitleBar);

		for(auto& widget : mWidgets)
		{
			EditorWidget::destroy(widget);
		}
	}

	void EditorWidgetContainer::add(EditorWidget& widget)
	{
		insert((UINT32)mWidgets.size(), widget);
	}

	void EditorWidgetContainer::remove(EditorWidget& widget)
	{
		INT32 widgetIdx = -1;
		UINT32 curIdx = 0;
		for(auto& curWidget : mWidgets)
		{
			if(curWidget == &widget)
			{
				widgetIdx = curIdx;
				break;
			}

			curIdx++;
		}

		if(widgetIdx == -1)
			return;

		mWidgets.erase(mWidgets.begin() + widgetIdx);
		mTitleBar->removeTab((UINT32)widgetIdx);
		widget._changeParent(nullptr);

		if(widgetIdx == mActiveWidget)
		{
			if(mWidgets.size() > 0)
			{
				setActiveWidget(0);
			}
		}
	}

	void EditorWidgetContainer::insert(CM::UINT32 idx, EditorWidget& widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget == &widget)
				return;
		}

		idx = Math::clamp(idx, 0U, (UINT32)mWidgets.size());

		mTitleBar->insertTab(idx, widget.getName());
		mWidgets.insert(mWidgets.begin() + idx, &widget);
		widget._changeParent(this);

		if(mActiveWidget == -1)
			setActiveWidget((UINT32)mWidgets.size() - 1);
		else
			widget._disable();
	}

	// Note: Hiding is used for a special purpose right now and there is no way to unhide a widget
	// (But such functionally may be easily added)
	void EditorWidgetContainer::hide(EditorWidget& widget)
	{
		INT32 widgetIdx = -1;
		UINT32 curIdx = 0;
		for(auto& curWidget : mWidgets)
		{
			if(curWidget == &widget)
			{
				widgetIdx = curIdx;
				break;
			}

			curIdx++;
		}

		if(widgetIdx == -1)
			return;

		mTitleBar->removeTab(widgetIdx);

		if(widgetIdx == mActiveWidget)
		{
			if(mWidgets.size() > 0)
			{
				setActiveWidget(0);
			}
		}
	}

	void EditorWidgetContainer::setSize(UINT32 width, UINT32 height)
	{
		// TODO - Title bar is always TitleBarHeight size, so what happens when the container area is smaller than that?
		mTitleBar->setSize(width, TitleBarHeight);

		if(mActiveWidget >= 0)
		{
			EditorWidget* activeWidgetPtr = mWidgets[mActiveWidget];
			UINT32 contentHeight = (UINT32)std::max(0, (INT32)height - (INT32)TitleBarHeight);

			activeWidgetPtr->_setSize(width, contentHeight);
		}

		mWidth = width;
		mHeight = height;
	}

	void EditorWidgetContainer::setPosition(INT32 x, INT32 y)
	{
		mTitleBar->setPosition(x, y);

		if(mActiveWidget >= 0)
		{
			EditorWidget* activeWidgetPtr = mWidgets[mActiveWidget];

			activeWidgetPtr->_setPosition(x, y + TitleBarHeight);
		}

		mX = x;
		mY = y;
	}

	void EditorWidgetContainer::setActiveWidget(UINT32 idx)
	{
		if(mActiveWidget == idx)
			return;

		mActiveWidget = idx;

		UINT32 curIdx = 0;
		for(auto& curWidget : mWidgets)
		{
			if(curIdx != (UINT32)mActiveWidget)
				curWidget->_disable();
			else
				curWidget->_enable();

			curIdx++;
		}

		setPosition(mX, mY);
		setSize(mWidth, mHeight);
	}

	void EditorWidgetContainer::tabActivated(UINT32 idx)
	{
		setActiveWidget(idx);
	}

	void EditorWidgetContainer::tabClosed(UINT32 idx)
	{
		EditorWidget* widget = mWidgets[idx];
		remove(*widget);
		EditorWidget::destroy(widget);

		if(!onWidgetClosed.empty())
			onWidgetClosed();
	}

	void EditorWidgetContainer::tabDraggedOff(CM::UINT32 idx)
	{
		EditorWidget* widget = mWidgets[idx];
		hide(*widget);

		// TODO - Hook up drag and drop texture
		DragAndDropManager::instance().startDrag(HTexture(), (UINT32)DragAndDropType::EditorWidget, (void*)widget, boost::bind(&EditorWidgetContainer::tabDroppedCallback, this, _1));

		// We don't want to remove the widget just yet. For now mark it as hidden in case
		// user just drops it somewhere else.
		// Note: This is primarily implemented because Windows doesn't like me destroying a window
		// while I'm capturing mouse input (required for drag and drop outside of window borders). 
		// So instead I just hide it and do the destroying after drag and drop is done.
		if(!onWidgetHidden.empty())
			onWidgetHidden();
	}

	void EditorWidgetContainer::tabDraggedOn(CM::UINT32 idx)
	{
#if CM_DEBUG_MODE
		if(!DragAndDropManager::instance().isDragInProgress())
			CM_EXCEPT(InternalErrorException, "Tab drag and drop reported but no drag in progress.");

		if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
			CM_EXCEPT(InternalErrorException, "Tab drag and drop reported but drag type is invalid.");
#endif

		EditorWidget* draggedWidget = static_cast<EditorWidget*>(DragAndDropManager::instance().getDragData());

		insert(idx, *draggedWidget);
	}

	void EditorWidgetContainer::tabDroppedCallback(bool wasDragProcessed)
	{
		if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
			return;

		EditorWidget* draggedWidget = static_cast<EditorWidget*>(DragAndDropManager::instance().getDragData());
		draggedWidget->_changeParent(nullptr); // To ensure it doesn't get destroyed with the parent window
		remove(*draggedWidget);

		if(!onWidgetClosed.empty())
			onWidgetClosed();

		if(!wasDragProcessed)
		{
			EditorWindow* newWindow = EditorWindow::create();

			newWindow->widgets().add(*draggedWidget);

			Int2 mousePos = Input::instance().getCursorPosition();
			newWindow->setPosition(mousePos.x, mousePos.y);
		}
	}

	void EditorWidgetContainer::_notifyWidgetDestroyed(EditorWidget* widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget == widget)
			{
				remove(*widget);

				if(!onWidgetClosed.empty())
					onWidgetClosed();

				return;
			}
		}		
	}
}