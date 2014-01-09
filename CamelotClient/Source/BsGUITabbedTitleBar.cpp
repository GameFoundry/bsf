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
#include "CmMath.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUITabbedTitleBar::TAB_SPACING = 20;
	const UINT32 GUITabbedTitleBar::OPTION_BTN_SPACING = 3;

	GUITabbedTitleBar::GUITabbedTitleBar(BS::GUIWidget* parent, CM::RenderWindow* parentWindow)
		:mParentWindow(parentWindow), mMinBtn(nullptr), mCloseBtn(nullptr), mCloseBtnStyle(nullptr),
		mMinimizeBtnStyle(nullptr), mParentWidget(parent), mBackgroundArea(nullptr), mUniqueTabIdx(0), mActiveTabIdx(0),
		mDragInProgress(false), mDraggedBtn(nullptr), mDragBtnOffset(0), mInitialDragOffset(0)
	{
		mBackgroundArea = GUIArea::create(*parent, 0, 0, 1, 13, 9900);
		GUITexture* titleBarBg = GUITexture::create(*parent, parent->getSkin().getStyle("TitleBarBackground"));
		mBackgroundArea->getLayout().addSpace(1);
		mBackgroundArea->getLayout().addElement(titleBarBg);
		mBackgroundArea->getLayout().addSpace(1);

		mMinimizeBtnStyle = parent->getSkin().getStyle("WinMinimizeBtn");
		mCloseBtnStyle = parent->getSkin().getStyle("WinCloseBtn");

		mMinBtn = GUIButton::create(*parent, HString(L""), mMinimizeBtnStyle);
		mCloseBtn = GUIButton::create(*parent, HString(L""), mCloseBtnStyle);

		mCloseBtn->onClick.connect(boost::bind(&GUITabbedTitleBar::tabClosed, this));

		mTabToggleGroup = GUIToggle::createToggleGroup();
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{
		GUIArea::destroy(mBackgroundArea);

		GUIElement::destroy(mMinBtn);
		GUIElement::destroy(mCloseBtn);

		for(auto& tabButton : mTabButtons)
		{
			GUIElement::destroy(tabButton);
		}
	}

	void GUITabbedTitleBar::addTab(const CM::HString& name)
	{
		insertTab((UINT32)mTabButtons.size(), name);
	}

	void GUITabbedTitleBar::insertTab(UINT32 idx, const CM::HString& name)
	{
		GUITabButton* newTabToggle = GUITabButton::create(*mParentWidget, mTabToggleGroup, mUniqueTabIdx, name, EngineGUI::instance().getSkin().getStyle("TabbedBarBtn"));

		idx = Math::clamp(idx, 0U, (UINT32)mTabButtons.size());

		newTabToggle->onToggled.connect(boost::bind(&GUITabbedTitleBar::tabToggled, this, mUniqueTabIdx));
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

	void GUITabbedTitleBar::updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth)
	{
		CM::Vector<CM::RectI>::type nonClientAreas;

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

			Vector2I offset(curX, curY);
			if(mDragInProgress && btn == mDraggedBtn)
				offset.x = mDragBtnOffset;
			
			btn->_setOffset(offset);
			btn->_setWidth(optimalSize.x);
			btn->_setHeight(optimalSize.y);
			btn->_setAreaDepth(9899);
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
			mMinBtn->_setAreaDepth(9899);
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
			mCloseBtn->_setAreaDepth(9899);
			mCloseBtn->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mCloseBtn->_setClipRect(elemClipRect);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow, nonClientAreas);
	}

	void GUITabbedTitleBar::setPosition(INT32 x, INT32 y)
	{
		mArea.x = x;
		mArea.y = y;

		mBackgroundArea->setPosition(x, y);

		updateLayout(mArea.x, mArea.y, mArea.width, mArea.height, RectI(mArea.x, mArea.y, mArea.width, mArea.height), mParentWidget->getDepth());
	}

	void GUITabbedTitleBar::setSize(UINT32 width, UINT32 height)
	{
		mArea.width = width;
		mArea.height = height;

		mBackgroundArea->setSize(width, height);

		updateLayout(mArea.x, mArea.y, mArea.width, mArea.height, RectI(mArea.x, mArea.y, mArea.width, mArea.height), mParentWidget->getDepth());
	}

	void GUITabbedTitleBar::tabToggled(CM::UINT32 tabIdx)
	{
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
			if(mArea.contains(dragPos))
			{
				mDraggedBtn = mTabButtons[idx];

				if(!mDragInProgress)
				{
					Vector2I offset = mDraggedBtn->_getOffset();
					mInitialDragOffset = (dragPos.x - offset.x);

					mDragInProgress = true;
				}

				mDragBtnOffset = dragPos.x - mArea.x - mInitialDragOffset;
				
				Vector2I offset = mDraggedBtn->_getOffset();
				INT32 diff = mDragBtnOffset - offset.x;
				offset.x += diff;
				mDraggedBtn->_setOffset(offset);

				RectI clipRect = mDraggedBtn->_getClipRect();
				clipRect.x -= diff;
				mDraggedBtn->_setClipRect(clipRect);

				mDragInProgress = true;

				for(UINT32 i = 0; i < idx; i++)
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
			}
			else
			{
				mDragInProgress = false;
				mDraggedBtn = nullptr;

				if(!onTabDraggedOff.empty())
					onTabDraggedOff(idx);
			}
		}
	}

	void GUITabbedTitleBar::tabDragEnd(CM::UINT32 tabIdx, const Vector2I& dragPos)
	{
		mDragInProgress = false;
		mDraggedBtn = nullptr;

		updateLayout(mArea.x, mArea.y, mArea.width, mArea.height, RectI(mArea.x, mArea.y, mArea.width, mArea.height), mParentWidget->getDepth());
	}

	void GUITabbedTitleBar::tabDraggedOn(CM::UINT32 tabIdx)
	{
		INT32 idx = uniqueIdxToSeqIdx(tabIdx);
		if(idx != -1)
		{
			if(!onTabDraggedOn.empty())
				onTabDraggedOn(idx + 1);
		}
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
}