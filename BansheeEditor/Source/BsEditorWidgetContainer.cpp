#include "BsEditorWidgetContainer.h"
#include "BsGUITabbedTitleBar.h"
#include "BsEditorWidget.h"
#include "BsDragAndDropManager.h"
#include "BsEditorWindow.h"
#include "BsGUIArea.h"
#include "CmMath.h"
#include "CmInput.h"
#include "BsGUIWidget.h"
#include "BsGUILayout.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 EditorWidgetContainer::TitleBarHeight = 13;

	EditorWidgetContainer::EditorWidgetContainer(BS::GUIWidget* parent, RenderWindow* renderWindow, EditorWindow* parentEditorWindow)
		:mParent(parent), mX(0), mY(0), mWidth(0), mHeight(0), mTitleBar(nullptr), mActiveWidget(-1),
		mTitleBarArea(nullptr), mParentWindow(parentEditorWindow)
	{
		mTitleBarArea = GUIArea::create(*parent, 0, 0, 0, 0, 9900);

		mTitleBar = GUITabbedTitleBar::create(*parent, renderWindow);
		mTitleBar->onTabActivated.connect(boost::bind(&EditorWidgetContainer::tabActivated, this, _1));
		mTitleBar->onTabClosed.connect(boost::bind(&EditorWidgetContainer::tabClosed, this, _1));
		mTitleBar->onTabDraggedOff.connect(boost::bind(&EditorWidgetContainer::tabDraggedOff, this, _1));
		mTitleBar->onTabDraggedOn.connect(boost::bind(&EditorWidgetContainer::tabDraggedOn, this, _1));

		mTitleBarArea->getLayout().addElement(mTitleBar);
	}

	EditorWidgetContainer::~EditorWidgetContainer()
	{
		for(auto& widget : mWidgets)
		{
			widget.second->close();
		}

		GUIArea::destroy(mTitleBarArea);
		GUIElement::destroy(mTitleBar);
	}

	void EditorWidgetContainer::add(EditorWidgetBase& widget)
	{
		insert((UINT32)mWidgets.size(), widget);
	}

	void EditorWidgetContainer::remove(EditorWidgetBase& widget)
	{
		removeInternal(widget);

		if(!onWidgetClosed.empty())
			onWidgetClosed();
	}

	void EditorWidgetContainer::removeInternal(EditorWidgetBase& widget)
	{
		INT32 tabIdx = 0;
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.second == &widget)
			{
				tabIdx = curWidget.first;
				break;
			}
		}

		if(tabIdx == -1)
			return;

		mWidgets.erase((UINT32)tabIdx);
		mTitleBar->removeTab((UINT32)tabIdx);
		widget._changeParent(nullptr);

		if(tabIdx == mActiveWidget)
		{
			mActiveWidget = -1;
			if(mTitleBar->getNumTabs() > 0)
			{
				setActiveWidget(mTitleBar->getTabIdx(0));
			}
		}
	}

	void EditorWidgetContainer::insert(CM::UINT32 idx, EditorWidgetBase& widget)
	{
		if(contains(widget))
			return;

		idx = Math::clamp(idx, 0U, (UINT32)mWidgets.size());

		UINT32 tabIdx = mTitleBar->insertTab(idx, widget.getDisplayName());
		mWidgets[tabIdx] = &widget;
		widget._changeParent(this);

		if(mActiveWidget == -1)
			setActiveWidget(mTitleBar->getTabIdx(mTitleBar->getNumTabs() - 1));
		else
			widget._disable();
	}

	bool EditorWidgetContainer::contains(EditorWidgetBase& widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.second == &widget)
				return true;
		}

		return false;
	}

	EditorWidgetBase* EditorWidgetContainer::getWidget(CM::UINT32 idx) const
	{
		if(idx >= (UINT32)mWidgets.size())
			return nullptr;

		UINT32 tabIdx = mTitleBar->getTabIdx(idx);

		auto iterFind = mWidgets.find(tabIdx);

		if(iterFind != mWidgets.end())
			return iterFind->second;

		return nullptr;
	}

	void EditorWidgetContainer::setSize(UINT32 width, UINT32 height)
	{
		// TODO - Title bar is always TitleBarHeight size, so what happens when the container area is smaller than that?
		mTitleBarArea->setSize(width, TitleBarHeight);

		if(mActiveWidget >= 0)
		{
			EditorWidgetBase* activeWidgetPtr = mWidgets[mActiveWidget];
			UINT32 contentHeight = (UINT32)std::max(0, (INT32)height - (INT32)TitleBarHeight);

			activeWidgetPtr->_setSize(width, contentHeight);
		}

		mWidth = width;
		mHeight = height;
	}

	void EditorWidgetContainer::setPosition(INT32 x, INT32 y)
	{
		mTitleBarArea->setPosition(x, y);

		if(mActiveWidget >= 0)
		{
			EditorWidgetBase* activeWidgetPtr = mWidgets[mActiveWidget];

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

		for(auto& curWidget : mWidgets)
		{
			if(curWidget.first != (UINT32)mActiveWidget)
				curWidget.second->_disable();
			else
				curWidget.second->_enable();
		}

		mTitleBar->setActive(idx);

		setPosition(mX, mY);
		setSize(mWidth, mHeight);
	}

	void EditorWidgetContainer::tabActivated(UINT32 uniqueIdx)
	{
		setActiveWidget(uniqueIdx);
	}

	void EditorWidgetContainer::tabClosed(UINT32 uniqueIdx)
	{
		EditorWidgetBase* widget = mWidgets[uniqueIdx];
		widget->close();

		if(!onWidgetClosed.empty())
			onWidgetClosed();
	}

	void EditorWidgetContainer::tabDraggedOff(CM::UINT32 uniqueIdx)
	{
		EditorWidgetBase* widget = mWidgets[uniqueIdx];
		removeInternal(*widget);

		// TODO - Hook up drag and drop texture
		DragAndDropManager::instance().startDrag(HTexture(), (UINT32)DragAndDropType::EditorWidget, (void*)widget, 
			boost::bind(&EditorWidgetContainer::tabDroppedCallback, _1));

		if(!onWidgetClosed.empty())
			onWidgetClosed();
	}

	void EditorWidgetContainer::tabDraggedOn(CM::UINT32 seqIdx)
	{
#if CM_DEBUG_MODE
		if(!DragAndDropManager::instance().isDragInProgress())
			CM_EXCEPT(InternalErrorException, "Tab drag and drop reported but no drag in progress.");

		if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
			CM_EXCEPT(InternalErrorException, "Tab drag and drop reported but drag type is invalid.");
#endif

		EditorWidgetBase* draggedWidget = static_cast<EditorWidgetBase*>(DragAndDropManager::instance().getDragData());

		insert(seqIdx, *draggedWidget);
		setActiveWidget(mTitleBar->getTabIdx(seqIdx));
	}

	void EditorWidgetContainer::tabDroppedCallback(bool wasDragProcessed)
	{
		if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
			return;

		if(!wasDragProcessed)
		{
			EditorWidgetBase* draggedWidget = static_cast<EditorWidgetBase*>(DragAndDropManager::instance().getDragData());
			EditorWindow* newWindow = EditorWindow::create();

			newWindow->widgets().add(*draggedWidget);

			Vector2I mousePos = Input::instance().getCursorPosition();
			newWindow->setPosition(mousePos.x, mousePos.y);
		}
	}

	RectI EditorWidgetContainer::getContentBounds() const
	{
		return RectI(mX, mY + TitleBarHeight, mWidth, (UINT32)std::max(0, (INT32)mHeight - (INT32)TitleBarHeight));
	}

	void EditorWidgetContainer::_notifyWidgetDestroyed(EditorWidgetBase* widget)
	{
		for(auto& curWidget : mWidgets)
		{
			if(curWidget.second == widget)
			{
				removeInternal(*widget);

				if(!onWidgetClosed.empty())
					onWidgetClosed();

				return;
			}
		}		
	}
}