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

	GUITabbedTitleBar::GUITabbedTitleBar(GUIWidget& parent, RenderWindow* parentWindow, GUIElementStyle* backgroundStyle, GUIElementStyle* tabBtnStyle, 
		GUIElementStyle* minBtnStyle, GUIElementStyle* closeBtnStyle, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, &GUISkin::DefaultStyle, layoutOptions, false), mParentWindow(parentWindow), mMinBtn(nullptr), 
		mCloseBtn(nullptr), mParentWidget(&parent), mBackgroundImage(nullptr), mUniqueTabIdx(0), mActiveTabIdx(0),
		mDragInProgress(false), mDraggedBtn(nullptr), mDragBtnOffset(0), mInitialDragOffset(0), mBackgroundStyle(backgroundStyle),
		mTabBtnStyle(tabBtnStyle), mMinimizeBtnStyle(minBtnStyle), mCloseBtnStyle(closeBtnStyle)
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
		mMinBtn = GUIButton::create(parent, HString(L""), mMinimizeBtnStyle);
		mCloseBtn = GUIButton::create(parent, HString(L""), mCloseBtnStyle);

		mCloseBtn->onClick.connect(boost::bind(&GUITabbedTitleBar::tabClosed, this));

		mTabToggleGroup = GUIToggle::createToggleGroup();
	}

	GUITabbedTitleBar::~GUITabbedTitleBar()
	{
		GUIElement::destroy(mBackgroundImage);

		GUIElement::destroy(mMinBtn);
		GUIElement::destroy(mCloseBtn);

		for(auto& tabButton : mTabButtons)
		{
			GUIElement::destroy(tabButton);
		}
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
		GUITabButton* newTabToggle = GUITabButton::create(*mParentWidget, mTabToggleGroup, mUniqueTabIdx, name, mTabBtnStyle);

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

	UINT32 GUITabbedTitleBar::getNumRenderElements() const
	{
		return 0;
	}

	const GUIMaterialInfo& GUITabbedTitleBar::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InvalidStateException, "Trying to retrieve a material from an element with no render elements.");
	}

	UINT32 GUITabbedTitleBar::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUITabbedTitleBar::updateClippedBounds()
	{
		mClippedBounds = RectI(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	void GUITabbedTitleBar::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	bool GUITabbedTitleBar::mouseEvent(const GUIMouseEvent& ev)
	{
		// TODO

		return false;
	}

	Vector2I GUITabbedTitleBar::_getOptimalSize() const
	{
		return Vector2I();
	}

	void GUITabbedTitleBar::_changeParentWidget(GUIWidget* widget)
	{
		GUIElement::_changeParentWidget(widget);

		mBackgroundImage->_changeParentWidget(widget);
		mMinBtn->_changeParentWidget(widget);
		mCloseBtn->_changeParentWidget(widget);

		for(auto& tabButton : mTabButtons)
		{
			tabButton->_changeParentWidget(widget);
		}
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
			mBackgroundImage->_setAreaDepth(areaDepth + 1);
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

			Vector2I offset(curX, curY);
			btn->_setOffset(offset);
			btn->_setWidth(optimalSize.x);
			btn->_setHeight(optimalSize.y);
			btn->_setAreaDepth(areaDepth);
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
			mMinBtn->_setAreaDepth(areaDepth);
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
			mCloseBtn->_setAreaDepth(areaDepth);
			mCloseBtn->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mCloseBtn->_setClipRect(elemClipRect);
		}

		Platform::setCaptionNonClientAreas(*mParentWindow, nonClientAreas);
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
			RectI bounds = getBounds();
			if(bounds.contains(dragPos))
			{
				mDraggedBtn = mTabButtons[idx];

				if(!mDragInProgress)
				{
					Vector2I offset = mDraggedBtn->_getOffset();
					mInitialDragOffset = (dragPos.x - offset.x);

					mDragInProgress = true;
				}

				mDragBtnOffset = dragPos.x - mInitialDragOffset;
				
				Vector2I offset = mDraggedBtn->_getOffset();
				INT32 diff = mDragBtnOffset - offset.x;
				offset.x += diff;
				mDraggedBtn->_setOffset(offset);

				RectI clipRect = mDraggedBtn->_getClipRect();
				clipRect.x -= diff;
				mDraggedBtn->_setClipRect(clipRect);

				mDragInProgress = true;

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

		markContentAsDirty();
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

	const String& GUITabbedTitleBar::getGUITypeName()
	{
		static String typeName = "TabbedTitleBar";
		return typeName;
	}
}