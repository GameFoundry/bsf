#include "BsGUITabbedTitleBar.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUITexture.h"
#include "BsGUIButton.h"
#include "BsGUITabButton.h"
#include "BsGUISpace.h"
#include "BsGUIWindowDropArea.h"
#include "BsEngineGUI.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsDragAndDropManager.h"
#include "BsEditorWidget.h"
#include "CmMath.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUITabbedTitleBar::TAB_SPACING = 20;
	const UINT32 GUITabbedTitleBar::OPTION_BTN_SPACING = 3;

	GUITabbedTitleBar::GUITabbedTitleBar(GUIWidget& parent, RenderWindow* parentWindow, GUIElementStyle* backgroundStyle, GUIElementStyle* tabBtnStyle, 
		GUIElementStyle* minBtnStyle, GUIElementStyle* closeBtnStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mParentWindow(parentWindow), mMinBtn(nullptr), 
		mCloseBtn(nullptr), mBackgroundImage(nullptr), mUniqueTabIdx(0), mActiveTabIdx(0),
		mDragInProgress(false), mDraggedBtn(nullptr), mDragBtnOffset(0), mInitialDragOffset(0), mBackgroundStyle(backgroundStyle),
		mTabBtnStyle(tabBtnStyle), mMinimizeBtnStyle(minBtnStyle), mCloseBtnStyle(closeBtnStyle), mTempDraggedWidget(nullptr),
		mTempDraggedTabIdx(0)
	{
		if(mBackgroundStyle == nullptr)
			mBackgroundStyle = parent.getSkin().getStyle("TitleBarBackground");

		if(mMinimizeBtnStyle == nullptr)
			mMinimizeBtnStyle = parent.getSkin().getStyle("WinMinimizeBtn");

		if(mCloseBtnStyle == nullptr)
			mCloseBtnStyle = parent.getSkin().getStyle("WinCloseBtn");

		if(mTabBtnStyle == nullptr)
			mTabBtnStyle = parent.getSkin().getStyle("TabbedBarBtn");

		mBackgroundImage = GUITexture::create(parent, mBackgroundStyle);
		_registerChildElement(mBackgroundImage);

		mMinBtn = GUIButton::create(parent, HString(L""), mMinimizeBtnStyle);
		_registerChildElement(mMinBtn);

		mCloseBtn = GUIButton::create(parent, HString(L""), mCloseBtnStyle);
		_registerChildElement(mCloseBtn);

		mCloseBtn->onClick.connect(boost::bind(&GUITabbedTitleBar::tabClosed, this));

		mTabToggleGroup = GUIToggle::createToggleGroup();
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{

	}

	GUITabbedTitleBar* GUITabbedTitleBar::create(GUIWidget& parent, RenderWindow* parentWindow, GUIElementStyle* backgroundStyle, 
		GUIElementStyle* tabBtnStyle, GUIElementStyle* minBtnStyle, GUIElementStyle* closeBtnStyle)
	{
		return new (cm_alloc<GUITabbedTitleBar, PoolAlloc>()) GUITabbedTitleBar(parent, parentWindow, backgroundStyle, tabBtnStyle, 
			minBtnStyle, closeBtnStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUITabbedTitleBar* GUITabbedTitleBar::create(GUIWidget& parent, RenderWindow* parentWindow, const GUILayoutOptions& layoutOptions)
	{
		return new (cm_alloc<GUITabbedTitleBar, PoolAlloc>()) GUITabbedTitleBar(parent, parentWindow, nullptr, nullptr, 
			nullptr, nullptr, layoutOptions);
	}

	GUITabbedTitleBar* GUITabbedTitleBar::create(GUIWidget& parent, RenderWindow* parentWindow, const GUILayoutOptions& layoutOptions, 
		GUIElementStyle* backgroundStyle, GUIElementStyle* tabBtnStyle, GUIElementStyle* minBtnStyle, GUIElementStyle* closeBtnStyle)
	{
		return new (cm_alloc<GUITabbedTitleBar, PoolAlloc>()) GUITabbedTitleBar(parent, parentWindow, backgroundStyle, tabBtnStyle, 
			minBtnStyle, closeBtnStyle, layoutOptions);
	}

	void GUITabbedTitleBar::addTab(const CM::HString& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	void GUITabbedTitleBar::insertTab(UINT32 idx, const CM::HString& name)
	{
		GUITabButton* newTabToggle = GUITabButton::create(*mParent, mTabToggleGroup, mUniqueTabIdx, name, mTabBtnStyle);
		_registerChildElement(newTabToggle);

		idx = Math::clamp(idx, 0U, (UINT32)mTabButtons.size());

		newTabToggle->onToggled.connect(boost::bind(&GUITabbedTitleBar::tabToggled, this, mUniqueTabIdx, _1));
		newTabToggle->onDragged.connect(boost::bind(&GUITabbedTitleBar::tabDragged, this, _1, _2));
		newTabToggle->onDragEnd.connect(boost::bind(&GUITabbedTitleBar::tabDragEnd, this, _1, _2));

		mTabButtons.insert(mTabButtons.begin() + idx, newTabToggle);

		mUniqueTabIdx++;
	}

	void GUITabbedTitleBar::removeTab(UINT32 idx)
	{
		if(mTabButtons.size() == 0)
			return;

		idx = Math::clamp(idx, 0U, (UINT32)mTabButtons.size() - 1);

		GUIElement::destroy(mTabButtons[idx]);

		mTabButtons.erase(mTabButtons.begin() + idx);
	}

	void GUITabbedTitleBar::setActive(UINT32 idx)
	{
		mTabButtons[idx]->toggleOn();
	}

	bool GUITabbedTitleBar::mouseEvent(const GUIMouseEvent& event)
	{
		if(event.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			EditorWidget* draggedWidget = reinterpret_cast<EditorWidget*>(DragAndDropManager::instance().getDragData());
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
						if(widgetRelPos.x >= centerX)
						{
							addTab(draggedWidget->getName());
							mTempDraggedTabIdx = mTabButtons[i + 1]->getIndex();

							break;
						}
					}
					else
					{
						if(i == 0 && widgetRelPos.x <= centerX)
						{
							insertTab(0, draggedWidget->getName());
							mTempDraggedTabIdx = mTabButtons[0]->getIndex();

							break;
						}
						else
						{
							UINT32 nextWidth = mTabButtons[i + 1]->_getWidth();
							INT32 nextCenterX = mTabButtons[i + 1]->_getOffset().x + nextWidth / 2;

							if(widgetRelPos.x > centerX && widgetRelPos.x < nextCenterX)
							{
								insertTab(i + 1, draggedWidget->getName());
								mTempDraggedTabIdx = mTabButtons[i + 1]->getIndex();

								break;
							}
						}
					}
				}

				mTempDraggedWidget = draggedWidget;

				mDraggedBtn = mTabButtons[uniqueIdxToSeqIdx(mTempDraggedTabIdx)];
				mDraggedBtn->_setDraggedState(true);

				mInitialDragOffset = Math::roundToInt(mDraggedBtn->_getWidth() * 0.5f);
				mDragInProgress = true;
			}

			if(mTempDraggedWidget != nullptr)
				tabDragged(mTempDraggedTabIdx, widgetRelPos);

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			EditorWidget* draggedWidget = reinterpret_cast<EditorWidget*>(DragAndDropManager::instance().getDragData());
			const Vector2I& widgetRelPos = event.getPosition();

			if(mTempDraggedWidget != nullptr)
			{
				UINT32 tabSeqIdx = uniqueIdxToSeqIdx(mTempDraggedTabIdx);
				removeTab(tabSeqIdx);

				mTempDraggedWidget = nullptr;
				mDragInProgress = false;
				mDraggedBtn = nullptr;

				if(!onTabDraggedOn.empty())
					onTabDraggedOn(tabSeqIdx);
			}

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseOut)
		{
			if(mTempDraggedWidget != nullptr)
			{
				removeTab(uniqueIdxToSeqIdx(mTempDraggedTabIdx));

				mTempDraggedWidget = nullptr;
				mDragInProgress = false;
				mDraggedBtn = nullptr;
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
		CM::Vector<CM::RectI>::type nonClientAreas;

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
			nonClientAreas.push_back(RectI(curX, curY, TAB_SPACING, tabBtnHeight));
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

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			btn->_setClipRect(elemClipRect);

			curX += optimalSize.x;
		}

		Vector2I minBtnOptimalSize = mMinBtn->_getOptimalSize();
		Vector2I closeBtnOptimalSize = mCloseBtn->_getOptimalSize();

		UINT32 endButtonWidth = minBtnOptimalSize.x + closeBtnOptimalSize.x + OPTION_BTN_SPACING;
		UINT32 remainingWidth = (UINT32)std::max(0, (INT32)(width - curX - endButtonWidth - 1));
		nonClientAreas.push_back(RectI(curX, curY, remainingWidth, tabBtnHeight));

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

		Platform::setCaptionNonClientAreas(*mParentWindow, nonClientAreas);
	}

	void GUITabbedTitleBar::tabToggled(CM::UINT32 tabIdx, bool toggledOn)
	{
		if(!toggledOn)
			return;

		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			if(!onTabActivated.empty())
				onTabActivated(idx);
		}

		mActiveTabIdx = tabIdx;
	}

	void GUITabbedTitleBar::tabClosed()
	{
		INT32 idx = uniqueIdxToSeqIdx(mActiveTabIdx);
		if(idx != -1)
		{
			removeTab(idx);

			if(mTabButtons.size() > 0)
				mActiveTabIdx = mTabButtons[0]->getIndex();

			if(!onTabClosed.empty())
				onTabClosed(idx);
		}
	}

	void GUITabbedTitleBar::tabDragged(CM::UINT32 tabIdx, const Vector2I& dragPos)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			RectI bounds = getBounds();
			if(bounds.contains(dragPos))
			{
				if(!mDragInProgress)
				{
					mDraggedBtn = mTabButtons[idx];
					mDraggedBtn->_setDraggedState(false);

					Vector2I offset = mDraggedBtn->_getOffset();
					mInitialDragOffset = (dragPos.x - offset.x);

					mDragInProgress = true;
				}

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
				mDragInProgress = false;

				if(mDraggedBtn != nullptr)
					mDraggedBtn->_setDraggedState(false);
				mDraggedBtn = nullptr;

				markContentAsDirty();

				if(!onTabDraggedOff.empty())
					onTabDraggedOff(idx);
			}
		}
	}

	void GUITabbedTitleBar::tabDragEnd(CM::UINT32 tabIdx, const Vector2I& dragPos)
	{
		mDragInProgress = false;

		if(mDraggedBtn != nullptr)
			mDraggedBtn->_setDraggedState(false);
		mDraggedBtn = nullptr;

		if(mActiveTabIdx != tabIdx)
			tabToggled(tabIdx, true);

		markContentAsDirty();
	}

	CM::INT32 GUITabbedTitleBar::uniqueIdxToSeqIdx(CM::UINT32 uniqueIdx) const
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