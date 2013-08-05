#include "BsEditorWidgetContainer.h"
#include "BsGUITabbedTitleBar.h"
#include "BsEditorWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 EditorWidgetContainer::TitleBarHeight = 13;

	EditorWidgetContainer::EditorWidgetContainer(BS::GUIWidget* parent)
		:mParent(parent), mX(0), mY(0), mWidth(0), mHeight(0), mTitleBar(nullptr), mActiveWidget(-1)
	{
		mTitleBar = cm_new<GUITabbedTitleBar>(parent);
		mTitleBar->onTabActivated.connect(boost::bind(&EditorWidgetContainer::tabActivated, this, _1));
		mTitleBar->onTabClosed.connect(boost::bind(&EditorWidgetContainer::tabClosed, this, _1));
	}

	EditorWidgetContainer::~EditorWidgetContainer()
	{
		cm_delete(mTitleBar);

		for(auto& widget : mWidgets)
		{
			EditorWidget::destroy(widget.widget);
		}
	}

	void EditorWidgetContainer::add(EditorWidget& widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.widget == &widget)
				return;
		}

		auto conn = widget.onClosed.connect(boost::bind(&EditorWidgetContainer::widgetDestroyed, this, _1));

		mTitleBar->addTab(widget.getName());
		mWidgets.push_back(WidgetInfo(&widget, conn));
		widget._changeParent(*mParent);

		if(mActiveWidget == -1)
		{
			setActiveWidget((UINT32)mWidgets.size() - 1);
		}
		else
			widget._disable();
	}

	void EditorWidgetContainer::remove(EditorWidget& widget)
	{
		INT32 widgetIdx = -1;
		UINT32 curIdx = 0;
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.widget == &widget)
			{
				widgetIdx = curIdx;
				break;
			}

			curIdx++;
		}

		if(widgetIdx == -1)
			return;

		mWidgets[widgetIdx].conn.disconnect();
		mWidgets.erase(mWidgets.begin() + widgetIdx);
		mTitleBar->removeTab((UINT32)widgetIdx);

		if(widgetIdx == mActiveWidget)
		{
			if(mWidgets.size() > 0)
			{
				setActiveWidget(0);
			}
		}
	}

	void EditorWidgetContainer::move(EditorWidget& widget, UINT32 newPosition)
	{
		// TODO
	}

	void EditorWidgetContainer::setSize(UINT32 width, UINT32 height)
	{
		// TODO - Title bar is always TitleBarHeight size, so what happens when the container area is smaller than that?
		mTitleBar->setSize(width, TitleBarHeight);

		if(mActiveWidget >= 0)
		{
			EditorWidget* activeWidgetPtr = mWidgets[mActiveWidget].widget;
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
			EditorWidget* activeWidgetPtr = mWidgets[mActiveWidget].widget;

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
				curWidget.widget->_disable();
			else
				curWidget.widget->_enable();

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
		EditorWidget* widget = mWidgets[idx].widget;
		remove(*widget);
		EditorWidget::destroy(widget);

		if(!onWidgetClosed.empty())
			onWidgetClosed();
	}

	void EditorWidgetContainer::widgetDestroyed(EditorWidget* widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.widget == widget)
			{
				remove(*widget);

				if(!onWidgetClosed.empty())
					onWidgetClosed();

				return;
			}
		}		
	}
}