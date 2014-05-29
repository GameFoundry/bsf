#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUITabButton.h"
#include "BsGUISpace.h"
#include "BsGUIWindowDropArea.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsDragAndDropManager.h"
#include "BsEditorWidget.h"
#include "BsMath.h"
#include "BsPlatform.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUITabbedTitleBar::TAB_SPACING = 20;
	const UINT32 GUITabbedTitleBar::OPTION_BTN_SPACING = 3;

	GUITabbedTitleBar::GUITabbedTitleBar(RenderWindow* parentWindow, const String& backgroundStyle, const String& tabBtnStyle, 
		const String& minBtnStyle, const String& closeBtnStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions), mParentWindow(parentWindow), mMinBtn(nullptr), 
		mCloseBtn(nullptr), mBackgroundImage(nullptr), mUniqueTabIdx(0), mActiveTabIdx(0),
		mDragInProgress(false), mDraggedBtn(nullptr), mDragBtnOffset(0), mInitialDragOffset(0), mBackgroundStyle(backgroundStyle),
		mTabBtnStyle(tabBtnStyle), mMinimizeBtnStyle(minBtnStyle), mCloseBtnStyle(closeBtnStyle), mTempDraggedWidget(nullptr),
		mTempDraggedTabIdx(0)
	{
		if(mBackgroundStyle == StringUtil::BLANK)
			mBackgroundStyle = "TitleBarBackground";

		if(mMinimizeBtnStyle == StringUtil::BLANK)
			mMinimizeBtnStyle = "WinMinimizeBtn";

		if(mCloseBtnStyle == StringUtil::BLANK)
			mCloseBtnStyle = "WinCloseBtn";

		if(mTabBtnStyle == StringUtil::BLANK)
			mTabBtnStyle = "TabbedBarBtn";

		mBackgroundImage = GUITexture::create(mBackgroundStyle);
		_registerChildElement(mBackgroundImage);

		mMinBtn = GUIButton::create(HString(L""), mMinimizeBtnStyle);
		_registerChildElement(mMinBtn);

		mCloseBtn = GUIButton::create(HString(L""), mCloseBtnStyle);
		_registerChildElement(mCloseBtn);

		mCloseBtn->onClick.connect(std::bind(&GUITabbedTitleBar::tabClosed, this));

		mTabToggleGroup = GUIToggle::createToggleGroup();
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{

	}

	GUITabbedTitleBar* GUITabbedTitleBar::create(RenderWindow* parentWindow, const String& backgroundStyle, 
		const String& tabBtnStyle, const String& minBtnStyle, const String& closeBtnStyle)
	{
		return new (bs_alloc<GUITabbedTitleBar, PoolAlloc>()) GUITabbedTitleBar(parentWindow, backgroundStyle, tabBtnStyle, 
			minBtnStyle, closeBtnStyle, GUILayoutOptions::create());
	}

	GUITabbedTitleBar* GUITabbedTitleBar::create(RenderWindow* parentWindow, const GUILayoutOptions& layoutOptions, 
		const String& backgroundStyle, const String& tabBtnStyle, const String& minBtnStyle, const String& closeBtnStyle)
	{
		return new (bs_alloc<GUITabbedTitleBar, PoolAlloc>()) GUITabbedTitleBar(parentWindow, backgroundStyle, tabBtnStyle, 
			minBtnStyle, closeBtnStyle, layoutOptions);
	}

	void GUITabbedTitleBar::addTab(const HString& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	UINT32 GUITabbedTitleBar::insertTab(UINT32 position, const HString& name)
	{
		GUITabButton* newTabToggle = GUITabButton::create(mTabToggleGroup, mUniqueTabIdx, name, mTabBtnStyle);
		_registerChildElement(newTabToggle);

		position = Math::clamp(position, 0U, (UINT32)mTabButtons.size());

		UINT32 uniqueIdx = mUniqueTabIdx++;

		newTabToggle->onToggled.connect(std::bind(&GUITabbedTitleBar::tabToggled, this, uniqueIdx, _1));
		newTabToggle->onDragged.connect(std::bind(&GUITabbedTitleBar::tabDragged, this, _1, _2));
		newTabToggle->onDragEnd.connect(std::bind(&GUITabbedTitleBar::tabDragEnd, this, _1, _2));

		mTabButtons.insert(mTabButtons.begin() + position, newTabToggle);

		return uniqueIdx;
	}

	void GUITabbedTitleBar::removeTab(UINT32 uniqueIdx)
	{
		INT32 idx = uniqueIdxToSeqIdx(uniqueIdx);
		if(idx == -1)
			return;

		idx = (INT32)Math::clamp((UINT32)idx, 0U, (UINT32)mTabButtons.size() - 1);

		GUIElement::destroy(mTabButtons[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
	}

	void GUITabbedTitleBar::setActive(UINT32 uniqueIdx)
	{
		mTabButtons[uniqueIdxToSeqIdx(uniqueIdx)]->toggleOn();
	}

	UINT32 GUITabbedTitleBar::getTabIdx(UINT32 position) const
	{
		return mTabButtons[position]->getIndex();
	}

	bool GUITabbedTitleBar::mouseEvent(const GUIMouseEvent& event)
	{
		if(event.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			EditorWidgetBase* draggedWidget = reinterpret_cast<EditorWidgetBase*>(DragAndDropManager::instance().getDragData());
			const Vector2I& widgetRelPos = event.getPosition();

			if(mTempDraggedWidget == nullptr)
			{
				UINT32 numTabButtons = (UINT32)mTabButtons.size();
				for(UINT32 i = 0; i < numTabButtons; i++)
				{
					UINT32 width = mTabButtons[i]->_getWidth();
					INT32 centerX = mTabButtons[i]->_getOffset().x + width / 2;

					if((i + 1) == numTabButtons)
					{
						if(i == 0 && widgetRelPos.x <= centerX)
						{
							insertTab(0, draggedWidget->getDisplayName());
							mTempDraggedTabIdx = mTabButtons[0]->getIndex();

							break;
						}
						else if(widgetRelPos.x > centerX)
						{
							addTab(draggedWidget->getDisplayName());
							mTempDraggedTabIdx = mTabButtons[i + 1]->getIndex();

							break;
						}
					}
					else
					{
						if(i == 0 && widgetRelPos.x <= centerX)
						{
							insertTab(0, draggedWidget->getDisplayName());
							mTempDraggedTabIdx = mTabButtons[0]->getIndex();

							break;
						}
						else
						{
							UINT32 nextWidth = mTabButtons[i + 1]->_getWidth();
							INT32 nextCenterX = mTabButtons[i + 1]->_getOffset().x + nextWidth / 2;

							if(widgetRelPos.x > centerX && widgetRelPos.x < nextCenterX)
							{
								insertTab(i + 1, draggedWidget->getDisplayName());
								mTempDraggedTabIdx = mTabButtons[i + 1]->getIndex();

								break;
							}
						}
					}
				}

				mTempDraggedWidget = draggedWidget;
				startDrag(uniqueIdxToSeqIdx(mTempDraggedTabIdx), Vector2I());
				mInitialDragOffset = Math::roundToInt(mDraggedBtn->_getOptimalSize().x * 0.5f);
			}

			if(mTempDraggedWidget != nullptr)
				tabDragged(mTempDraggedTabIdx, widgetRelPos);

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			EditorWidgetBase* draggedWidget = reinterpret_cast<EditorWidgetBase*>(DragAndDropManager::instance().getDragData());
			const Vector2I& widgetRelPos = event.getPosition();

			if(mTempDraggedWidget != nullptr)
			{
				UINT32 seqIdx = uniqueIdxToSeqIdx(mTempDraggedTabIdx);
				removeTab(mTempDraggedTabIdx);

				endDrag();

				if(!onTabDraggedOn.empty())
					onTabDraggedOn(seqIdx);
			}

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseOut)
		{
			if(mTempDraggedWidget != nullptr)
			{
				removeTab(mTempDraggedTabIdx);

				endDrag();
			}
		}

		return false;
	}

	void GUITabbedTitleBar::updateClippedBounds()
	{
		Vector2I offset = _getOffset();
		mClippedBounds = RectI(offset.x, offset.y, _getWidth(), _getHeight());
	}

	void GUITabbedTitleBar::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		Vector2I minBtnOptimalSize = mMinBtn->_getOptimalSize();
		Vector2I closeBtnOptimalSize = mCloseBtn->_getOptimalSize();

		UINT32 endButtonWidth = minBtnOptimalSize.x + closeBtnOptimalSize.x + OPTION_BTN_SPACING;

		RectI tabClipRect = clipRect;
		tabClipRect.width -= endButtonWidth;

		{
			Vector2I optimalSize = mBackgroundImage->_getOptimalSize();
			Vector2I offset(x + 1, y + 1);
			mBackgroundImage->_setOffset(offset);
			mBackgroundImage->_setWidth(width - 2);
			mBackgroundImage->_setHeight(optimalSize.y);
			mBackgroundImage->_setAreaDepth(areaDepth + 2);
			mBackgroundImage->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mBackgroundImage->_setClipRect(elemClipRect);
		}

		UINT32 curX = x + 1;
		UINT32 curY = y;
		UINT32 tabBtnHeight = 0;
		for(UINT32 i = 0; i < (UINT32)mTabButtons.size(); i++)
		{
			GUITabButton* btn = mTabButtons[i];
			Vector2I optimalSize = btn->_getOptimalSize();

			tabBtnHeight = optimalSize.y;
			curX += TAB_SPACING;

			Vector2I offset;
			if(!mDragInProgress || mDraggedBtn != btn)
			{
				offset = Vector2I(curX, curY);
			}
			else if(mDragInProgress && mDraggedBtn == btn)
			{
				offset = btn->_getOffset();
				offset.x = mDragBtnOffset;
			}

			btn->_setOffset(offset);
			btn->_setWidth(optimalSize.x);
			btn->_setHeight(optimalSize.y);
			btn->_setAreaDepth(areaDepth + 1);
			btn->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(tabClipRect.x - offset.x, tabClipRect.y - offset.y, tabClipRect.width, tabClipRect.height);
			btn->_setClipRect(elemClipRect);

			curX += optimalSize.x;
		}

		INT32 optionBtnXPos = x + width - endButtonWidth - 1;
		{
			INT32 optionBtnYPos = curY + Math::floorToInt((tabBtnHeight - minBtnOptimalSize.y) * 0.5f);

			Vector2I offset(optionBtnXPos, optionBtnYPos);
			mMinBtn->_setOffset(offset);
			mMinBtn->_setWidth(minBtnOptimalSize.x);
			mMinBtn->_setHeight(minBtnOptimalSize.y);
			mMinBtn->_setAreaDepth(areaDepth + 1);
			mMinBtn->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mMinBtn->_setClipRect(elemClipRect);
		}

		optionBtnXPos += minBtnOptimalSize.x + OPTION_BTN_SPACING;
		{
			INT32 optionBtnYPos = curY + Math::floorToInt((tabBtnHeight - closeBtnOptimalSize.y) * 0.5f);

			Vector2I offset(optionBtnXPos, optionBtnYPos);
			mCloseBtn->_setOffset(offset);
			mCloseBtn->_setWidth(closeBtnOptimalSize.x);
			mCloseBtn->_setHeight(closeBtnOptimalSize.y);
			mCloseBtn->_setAreaDepth(areaDepth + 1);
			mCloseBtn->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mCloseBtn->_setClipRect(elemClipRect);
		}
	}

	Vector<RectI> GUITabbedTitleBar::calcDraggableAreas(INT32 x, INT32 y, UINT32 width, UINT32 height) const
	{
		Vector<RectI> draggableAreas;

		UINT32 curX = x + 1;
		UINT32 curY = y;
		for(UINT32 i = 0; i < (UINT32)mTabButtons.size(); i++)
		{
			GUITabButton* btn = mTabButtons[i];
			Vector2I optimalSize = btn->_getOptimalSize();

			draggableAreas.push_back(RectI(curX, curY, TAB_SPACING, height));

			curX += TAB_SPACING + optimalSize.x;
		}

		Vector2I minBtnOptimalSize = mMinBtn->_getOptimalSize();
		Vector2I closeBtnOptimalSize = mCloseBtn->_getOptimalSize();

		UINT32 endButtonWidth = minBtnOptimalSize.x + closeBtnOptimalSize.x + OPTION_BTN_SPACING;
		UINT32 remainingWidth = (UINT32)std::max(0, (INT32)(width - curX - endButtonWidth - 1));

		if(remainingWidth > 0)
			draggableAreas.push_back(RectI(curX, curY, remainingWidth, height));

		return draggableAreas;
	}

	void GUITabbedTitleBar::tabToggled(UINT32 tabIdx, bool toggledOn)
	{
		if(!toggledOn)
			return;

		if(!onTabActivated.empty())
			onTabActivated(tabIdx);

		mActiveTabIdx = tabIdx;
	}

	void GUITabbedTitleBar::tabClosed()
	{
		removeTab(mActiveTabIdx);

		if(!onTabClosed.empty())
			onTabClosed(mActiveTabIdx);

		if(mTabButtons.size() > 0)
			mActiveTabIdx = mTabButtons[0]->getIndex();
	}

	void GUITabbedTitleBar::startDrag(UINT32 seqIdx, const Vector2I& startDragPos)
	{
		if(!mDragInProgress)
		{
			for(auto& btn : mTabButtons)
				btn->_setDraggedState(true);

			mDraggedBtn = mTabButtons[seqIdx];

			Vector2I offset = mDraggedBtn->_getOffset();
			mInitialDragOffset = (startDragPos.x - offset.x);

			mDragInProgress = true;
		}
	}

	void GUITabbedTitleBar::endDrag()
	{
		for(auto& btn : mTabButtons)
			btn->_setDraggedState(false);

		mTempDraggedWidget = nullptr;
		mDragInProgress = false;
		mDraggedBtn = nullptr;
	}

	void GUITabbedTitleBar::tabDragged(UINT32 tabIdx, const Vector2I& dragPos)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			RectI bounds = getBounds();
			if(bounds.contains(dragPos))
			{
				if(!mDragInProgress)
					startDrag(idx, dragPos);

				mDragBtnOffset = dragPos.x - mInitialDragOffset;

				for(INT32 i = 0; i < idx; i++)
				{
					UINT32 width = mTabButtons[i]->_getWidth();
					INT32 centerX = mTabButtons[i]->_getOffset().x + width / 2;

					if(dragPos.x < centerX)
					{
						GUITabButton* temp = mTabButtons[i];
						mTabButtons[i] = mTabButtons[idx];
						mTabButtons[idx] = temp;

						break;
					}
				}

				for(UINT32 i = idx + 1; i < (UINT32)mTabButtons.size(); i++)
				{
					UINT32 width = mTabButtons[i]->_getWidth();
					INT32 centerX = mTabButtons[i]->_getOffset().x + width / 2;

					if(dragPos.x > centerX)
					{
						GUITabButton* temp = mTabButtons[i];
						mTabButtons[i] = mTabButtons[idx];
						mTabButtons[idx] = temp;

						break;
					}
				}

				markContentAsDirty();
			}
			else
			{
				endDrag();
				markContentAsDirty();

				if(!onTabDraggedOff.empty())
					onTabDraggedOff(tabIdx);
			}
		}
	}

	void GUITabbedTitleBar::tabDragEnd(UINT32 tabIdx, const Vector2I& dragPos)
	{
		endDrag();

		if(mActiveTabIdx != tabIdx)
			tabToggled(tabIdx, true);

		markContentAsDirty();
	}

	INT32 GUITabbedTitleBar::uniqueIdxToSeqIdx(UINT32 uniqueIdx) const
	{
		UINT32 idx = 0;
		for(auto& tab : mTabButtons)
		{
			if(tab->getIndex() == uniqueIdx)
				return idx;

			idx++;
		}

		return -1;
	}

	const String& GUITabbedTitleBar::getGUITypeName()
	{
		static String typeName = "TabbedTitleBar";
		return typeName;
	}
}