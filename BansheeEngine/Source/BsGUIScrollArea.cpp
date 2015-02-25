#include "BsGUIScrollArea.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "BsGUISkin.h"
#include "BsGUIScrollBarVert.h"
#include "BsGUIScrollBarHorz.h"
#include "BsGUIMouseEvent.h"
#include "BsGUILayoutUtility.h"
#include "BsException.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIScrollArea::ScrollBarWidth = 8;
	const UINT32 GUIScrollArea::MinHandleSize = 4;
	const UINT32 GUIScrollArea::WheelScrollAmount = 50;

	GUIScrollArea::GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, 
		const String& scrollBarStyle, const String& scrollAreaStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions), mVertScroll(nullptr), mHorzScroll(nullptr), mVertOffset(0), mHorzOffset(0),
		mVertBarType(vertBarType), mHorzBarType(horzBarType), mScrollBarStyle(scrollBarStyle)
	{
		mContentLayout = &addLayoutYInternal(this);

		mHorzScroll = GUIScrollBarHorz::create(mScrollBarStyle);
		mVertScroll = GUIScrollBarVert::create(mScrollBarStyle);

		_registerChildElement(mHorzScroll);
		_registerChildElement(mVertScroll);

		mHorzScroll->onScrollPositionChanged.connect(std::bind(&GUIScrollArea::horzScrollUpdate, this, _1));
		mVertScroll->onScrollPositionChanged.connect(std::bind(&GUIScrollArea::vertScrollUpdate, this, _1));
	}

	GUIScrollArea::~GUIScrollArea()
	{

	}

	void GUIScrollArea::updateClippedBounds()
	{
		Rect2I bounds(0, 0, mWidth, mHeight);
		bounds.clip(mClipRect);
		bounds.x += mOffset.x;
		bounds.y += mOffset.y;

		mClippedBounds = bounds;
	}

	void GUIScrollArea::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, const Vector<LayoutSizeRange>& sizeRanges) const
	{
		Vector2I visibleSize, contentSize;
		_getElementAreas(x, y, width, height, elementAreas, numElements, sizeRanges, visibleSize, contentSize);
	}

	void GUIScrollArea::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, 
		const Vector<LayoutSizeRange>& sizeRanges, Vector2I& visibleSize, Vector2I& contentSize) const
	{
		assert(mChildren.size() == numElements && numElements == 3);

		UINT32 layoutIdx = 0;
		UINT32 horzScrollIdx = 0;
		UINT32 vertScrollIdx = 0;
		UINT32 idx = 0;
		for (auto& child : mChildren)
		{
			if (child == mContentLayout)
				layoutIdx = idx;

			if (child == mHorzScroll)
				horzScrollIdx = idx;

			if (child == mVertScroll)
				vertScrollIdx = idx;

			idx++;
		}

		// Calculate content layout bounds

		//// We want elements to use their optimal height, since scroll area
		//// technically provides "infinite" space
		UINT32 optimalContentWidth = width;
		if (mHorzBarType != ScrollBarType::NeverShow)
			optimalContentWidth = sizeRanges[layoutIdx].optimal.x;

		UINT32 optimalContentHeight = height;
		if (mVertBarType != ScrollBarType::NeverShow)
			optimalContentHeight = sizeRanges[layoutIdx].optimal.y;

		UINT32 layoutWidth = std::max(optimalContentWidth, width);
		UINT32 layoutHeight = std::max(optimalContentHeight, height);

		contentSize = GUILayoutUtility::calcActualSize(layoutWidth, layoutHeight, mContentLayout);
		visibleSize = Vector2I(width, height);

		bool addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && contentSize.x > visibleSize.x) ||
			mHorzBarType == ScrollBarType::AlwaysShow && mHorzBarType != ScrollBarType::NeverShow;

		bool hasHorzScrollbar = false;
		bool hasVertScrollbar = false;
		if (addHorzScrollbar)
		{
			// Make room for scrollbar
			visibleSize.y = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);

			if (mVertBarType == ScrollBarType::NeverShow)
				layoutHeight = (UINT32)visibleSize.y;
			else
				layoutHeight = std::max(optimalContentHeight, (UINT32)visibleSize.y); // Never go below optimal size

			contentSize = GUILayoutUtility::calcActualSize(layoutWidth, layoutHeight, mContentLayout);
			hasHorzScrollbar = true;
		}

		bool addVertScrollbar = (mVertBarType == ScrollBarType::ShowIfDoesntFit && contentSize.y > visibleSize.y) ||
			mVertBarType == ScrollBarType::AlwaysShow && mVertBarType != ScrollBarType::NeverShow;

		if (addVertScrollbar)
		{
			// Make room for scrollbar
			visibleSize.x = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			
			if (mHorzBarType == ScrollBarType::NeverShow)
				layoutWidth = (UINT32)visibleSize.x;
			else
				layoutWidth = std::max(optimalContentWidth, (UINT32)visibleSize.x); // Never go below optimal size

			contentSize = GUILayoutUtility::calcActualSize(layoutWidth, layoutHeight, mContentLayout);
			hasVertScrollbar = true;

			if (!hasHorzScrollbar) // Since width has been reduced, we need to check if we require the horizontal scrollbar
			{
				addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && contentSize.x > visibleSize.x) && mHorzBarType != ScrollBarType::NeverShow;

				if (addHorzScrollbar)
				{
					// Make room for scrollbar
					visibleSize.y = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);

					if (mVertBarType == ScrollBarType::NeverShow)
						layoutHeight = (UINT32)visibleSize.y;
					else
						layoutHeight = std::max(optimalContentHeight, (UINT32)visibleSize.y); // Never go below optimal size

					contentSize = GUILayoutUtility::calcActualSize(layoutWidth, layoutHeight, mContentLayout);
					hasHorzScrollbar = true;
				}
			}
		}

		elementAreas[layoutIdx] = Rect2I(x - Math::floorToInt(mHorzOffset), y - Math::floorToInt(mVertOffset), layoutWidth, layoutHeight);

		// Calculate vertical scrollbar bounds
		if (hasVertScrollbar)
		{
			INT32 scrollBarOffset = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			UINT32 scrollBarHeight = height;
			if (hasHorzScrollbar)
				scrollBarHeight = (UINT32)std::max(0, (INT32)scrollBarHeight - (INT32)ScrollBarWidth);

			elementAreas[vertScrollIdx] = Rect2I(x + scrollBarOffset, y, ScrollBarWidth, scrollBarHeight);
		}
		else
		{
			elementAreas[vertScrollIdx] = Rect2I(x + layoutWidth, y, 0, 0);
		}

		// Calculate horizontal scrollbar bounds
		if (hasHorzScrollbar)
		{
			INT32 scrollBarOffset = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
			UINT32 scrollBarWidth = width;
			if (hasVertScrollbar)
				scrollBarWidth = (UINT32)std::max(0, (INT32)scrollBarWidth - (INT32)ScrollBarWidth);

			elementAreas[horzScrollIdx] = Rect2I(x, y + scrollBarOffset, scrollBarWidth, ScrollBarWidth);
		}
		else
		{
			elementAreas[horzScrollIdx] = Rect2I(x, y + layoutHeight, 0, 0);
		}
	}

	void GUIScrollArea::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 numElements = (UINT32)mChildren.size();
		Rect2I* elementAreas = nullptr;

		if (numElements > 0)
			elementAreas = stackConstructN<Rect2I>(numElements);

		Vector<LayoutSizeRange> sizeRanges;
		UINT32 layoutIdx = 0;
		UINT32 horzScrollIdx = 0;
		UINT32 vertScrollIdx = 0;
		for (UINT32 i = 0; i < numElements; i++)
		{
			GUIElementBase* child = _getChild(i);
			sizeRanges.push_back(child->_calculateLayoutSizeRange());

			if (child == mContentLayout)
				layoutIdx = i;

			if (child == mHorzScroll)
				horzScrollIdx = i;

			if (child == mVertScroll)
				vertScrollIdx = i;
		}

		_getElementAreas(x, y, width, height, elementAreas, numElements, sizeRanges, mVisibleSize, mContentSize);

		Rect2I& layoutBounds = elementAreas[layoutIdx];
		Rect2I& horzScrollBounds = elementAreas[horzScrollIdx];
		Rect2I& vertScrollBounds = elementAreas[vertScrollIdx];

		// Layout
		Rect2I layoutClipRect = clipRect;
		layoutClipRect.width = (UINT32)mVisibleSize.x;
		layoutClipRect.height = (UINT32)mVisibleSize.y;
		mContentLayout->_updateLayoutInternal(layoutBounds.x, layoutBounds.y,
			layoutBounds.width, layoutBounds.height, layoutClipRect, widgetDepth, areaDepth);

		// Vertical scrollbar
		{
			mVertScroll->setOffset(Vector2I(vertScrollBounds.x, vertScrollBounds.y));
			mVertScroll->setWidth(vertScrollBounds.width);
			mVertScroll->setHeight(vertScrollBounds.height);
			mVertScroll->_setAreaDepth(areaDepth);
			mVertScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarWidth = std::min(width, ScrollBarWidth);
			Rect2I elemClipRect(0, 0, clippedScrollbarWidth, clipRect.height);
			mVertScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			Rect2I scrollBarLayoutClipRect(clipRect.x + (vertScrollBounds.x - x), clipRect.y + (vertScrollBounds.y - y), clippedScrollbarWidth, clipRect.height);
			mVertScroll->_updateLayout(vertScrollBounds.x, vertScrollBounds.y, vertScrollBounds.width, vertScrollBounds.height, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mVertScroll->getMaxHandleSize() * (vertScrollBounds.height / (float)mContentSize.y));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentSize.y) - INT32(vertScrollBounds.height));
			float newScrollPct = 0.0f;

			if (scrollableHeight > 0)
				newScrollPct = mVertOffset / scrollableHeight;

			mVertScroll->setHandleSize(newHandleSize);
			mVertScroll->setScrollPos(newScrollPct);
		}

		// Horizontal scrollbar
		{
			mHorzScroll->setOffset(Vector2I(horzScrollBounds.x, horzScrollBounds.y));
			mHorzScroll->setWidth(horzScrollBounds.width);
			mHorzScroll->setHeight(horzScrollBounds.height);
			mHorzScroll->_setAreaDepth(areaDepth);
			mHorzScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarHeight = std::min(height, ScrollBarWidth);
			Rect2I elemClipRect(0, 0, clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			Rect2I scrollBarLayoutClipRect(clipRect.x + (horzScrollBounds.x - x), clipRect.y + (horzScrollBounds.y - y), clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_updateLayout(horzScrollBounds.x, horzScrollBounds.y, horzScrollBounds.width, horzScrollBounds.height, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mHorzScroll->getMaxHandleSize() * (horzScrollBounds.width / (float)mContentSize.x));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableWidth = (UINT32)std::max(0, INT32(mContentSize.x) - INT32(horzScrollBounds.width));
			float newScrollPct = 0.0f;

			if (scrollableWidth > 0)
				newScrollPct = mHorzOffset / scrollableWidth;

			mHorzScroll->setHandleSize(newHandleSize);
			mHorzScroll->setScrollPos(newScrollPct);
		}

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);
	}

	void GUIScrollArea::vertScrollUpdate(float scrollPos)
	{
		scrollToVertical(scrollPos);
	}

	void GUIScrollArea::horzScrollUpdate(float scrollPos)
	{
		scrollToHorizontal(scrollPos);
	}

	void GUIScrollArea::scrollToVertical(float pct)
	{
		UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentSize.y) - INT32(mVisibleSize.y));
		mVertOffset = scrollableHeight * Math::clamp01(pct);

		markContentAsDirty();
	}

	void GUIScrollArea::scrollToHorizontal(float pct)
	{
		UINT32 scrollableWidth = (UINT32)std::max(0, INT32(mContentSize.x) - INT32(mVisibleSize.x));
		mHorzOffset = scrollableWidth * Math::clamp01(pct);

		markContentAsDirty();
	}

	void GUIScrollArea::scrollUpPx(UINT32 pixels)
	{
		if(mVertScroll != nullptr)
		{
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentSize.y) - INT32(mVisibleSize.y));

			float offset = 0.0f;
			if(scrollableSize > 0)
				offset = pixels / (float)scrollableSize;

			mVertScroll->scroll(offset);
		}
	}

	void GUIScrollArea::scrollDownPx(UINT32 pixels)
	{
		if(mVertScroll != nullptr)
		{
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentSize.y) - INT32(mVisibleSize.y));

			float offset = 0.0f;
			if(scrollableSize > 0)
				offset = pixels / (float)scrollableSize;

			mVertScroll->scroll(-offset);
		}
	}

	void GUIScrollArea::scrollLeftPx(UINT32 pixels)
	{
		if(mHorzScroll != nullptr)
		{
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentSize.x) - INT32(mVisibleSize.x));

			float offset = 0.0f;
			if(scrollableSize > 0)
				offset = pixels / (float)scrollableSize;

			mHorzScroll->scroll(offset);
		}
	}

	void GUIScrollArea::scrollRightPx(UINT32 pixels)
	{
		if(mHorzScroll != nullptr)
		{
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentSize.x) - INT32(mVisibleSize.x));

			float offset = 0.0f;
			if(scrollableSize > 0)
				offset = pixels / (float)scrollableSize;

			mHorzScroll->scroll(-offset);
		}
	}

	void GUIScrollArea::scrollUpPct(float percent)
	{
		if(mVertScroll != nullptr)
			mVertScroll->scroll(percent);
	}

	void GUIScrollArea::scrollDownPct(float percent)
	{
		if(mVertScroll != nullptr)
			mVertScroll->scroll(-percent);
	}

	void GUIScrollArea::scrollLeftPct(float percent)
	{
		if(mHorzScroll != nullptr)
			mHorzScroll->scroll(percent);
	}

	void GUIScrollArea::scrollRightPct(float percent)
	{
		if(mHorzScroll != nullptr)
			mHorzScroll->scroll(-percent);
	}

	bool GUIScrollArea::_mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseWheelScroll)
		{
			// Mouse wheel only scrolls on the Y axis
			if(mVertScroll != nullptr)
			{
				UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentSize.y) - INT32(mVisibleSize.y));
				float additionalScroll = (float)WheelScrollAmount / scrollableHeight;

				mVertScroll->scroll(additionalScroll * ev.getWheelScrollAmount());
				return true;
			}
		}

		return false;
	}

	GUIScrollArea* GUIScrollArea::create(ScrollBarType vertBarType, ScrollBarType horzBarType, 
		const String& scrollBarStyle, const String& scrollAreaStyle)
	{
		return new (bs_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(vertBarType, horzBarType, scrollBarStyle, 
			getStyleName<GUIScrollArea>(scrollAreaStyle), GUILayoutOptions::create());
	}

	GUIScrollArea* GUIScrollArea::create(const GUIOptions& layoutOptions, const String& scrollBarStyle, 
		const String& scrollAreaStyle)
	{
		return new (bs_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(ScrollBarType::ShowIfDoesntFit, 
			ScrollBarType::ShowIfDoesntFit, scrollBarStyle, getStyleName<GUIScrollArea>(scrollAreaStyle), GUILayoutOptions::create(layoutOptions));
	}

	GUIScrollArea* GUIScrollArea::create(const String& scrollBarStyle, const String& scrollAreaStyle)
	{
		return new (bs_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(ScrollBarType::ShowIfDoesntFit, ScrollBarType::ShowIfDoesntFit, scrollBarStyle, 
			getStyleName<GUIScrollArea>(scrollAreaStyle), GUILayoutOptions::create());
	}

	GUIScrollArea* GUIScrollArea::create(ScrollBarType vertBarType, 
		ScrollBarType horzBarType, const GUIOptions& layoutOptions, const String& scrollBarStyle, 
		const String& scrollAreaStyle)
	{
		return new (bs_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(vertBarType, horzBarType, scrollBarStyle, 
			getStyleName<GUIScrollArea>(scrollAreaStyle), GUILayoutOptions::create(layoutOptions));
	}

	const String& GUIScrollArea::getGUITypeName()
	{
		static String typeName = "ScrollArea";
		return typeName;
	}
}