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
		mContentWidth(0), mContentHeight(0), mClippedContentWidth(0), mClippedContentHeight(0), mVertBarType(vertBarType), mHorzBarType(horzBarType),
		mScrollBarStyle(scrollBarStyle)
	{
		mContentLayout = &addLayoutYInternal(this);
	}

	GUIScrollArea::~GUIScrollArea()
	{

	}

	void GUIScrollArea::updateClippedBounds()
	{
		RectI bounds(0, 0, mWidth, mHeight);
		bounds.clip(mClipRect);
		bounds.x += mOffset.x;
		bounds.y += mOffset.y;

		mClippedBounds = bounds;
	}

	void GUIScrollArea::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		// We want elements to use their optimal height, since scroll area
		// technically provides "infinite" space
		UINT32 contentLayoutWidth = width;
		if(mHorzBarType != ScrollBarType::NeverShow)
			contentLayoutWidth = mContentLayout->_getOptimalSize().x;

		UINT32 contentLayoutHeight = height;
		if(mVertBarType != ScrollBarType::NeverShow)
			contentLayoutHeight = mContentLayout->_getOptimalSize().y;

		mContentLayout->_updateLayoutInternal(x, y, contentLayoutWidth, contentLayoutHeight, clipRect, widgetDepth, areaDepth);
		mContentWidth = mContentLayout->_getActualWidth();
		mContentHeight = mContentLayout->_getActualHeight();

		mClippedContentWidth = width;
		mClippedContentHeight = height;

		RectI layoutClipRect = clipRect;
		bool addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && mContentWidth > mWidth) || 
			mHorzBarType == ScrollBarType::AlwaysShow && mHorzBarType != ScrollBarType::NeverShow;

		bool hasHorzScrollbar = false;
		bool hasVertScrollbar = false;
		if(addHorzScrollbar)
		{ 
			// Make room for scrollbar
			mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
			layoutClipRect.height = mClippedContentHeight;
			hasHorzScrollbar = true;

			if(mVertBarType == ScrollBarType::NeverShow)
				contentLayoutHeight = mClippedContentHeight;

			mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y, 
				contentLayoutWidth, contentLayoutHeight, layoutClipRect, widgetDepth, areaDepth);

			mContentWidth = mContentLayout->_getActualWidth();
			mContentHeight = mContentLayout->_getActualHeight();
		}

		bool addVertScrollbar = (mVertBarType == ScrollBarType::ShowIfDoesntFit && mContentHeight > mClippedContentHeight) || 
			mVertBarType == ScrollBarType::AlwaysShow && mVertBarType != ScrollBarType::NeverShow;

		if(addVertScrollbar)
		{
			// Make room for scrollbar
			mClippedContentWidth = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			layoutClipRect.width = mClippedContentWidth;
			hasVertScrollbar = true;

			if(mHorzBarType == ScrollBarType::NeverShow)
				contentLayoutWidth = mClippedContentWidth;

			if(hasHorzScrollbar)
			{
				mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y - Math::floorToInt(mVertOffset), 
					contentLayoutWidth, contentLayoutHeight, 
					layoutClipRect, widgetDepth, areaDepth);
			}
			else
			{
				mContentLayout->_updateLayoutInternal(x, y - Math::floorToInt(mVertOffset), 
					contentLayoutWidth, contentLayoutHeight, 
					layoutClipRect, widgetDepth, areaDepth);
			}

			mContentWidth = mContentLayout->_getActualWidth();
			mContentHeight = mContentLayout->_getActualHeight();

			if(!hasHorzScrollbar) // Since width has been reduced, we need to check if we require the horizontal scrollbar
			{
				addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && mContentWidth > mClippedContentWidth) && mHorzBarType != ScrollBarType::NeverShow;

				if(addHorzScrollbar)
				{
					// Make room for scrollbar
					mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
					layoutClipRect.height = mClippedContentHeight;

					if(mVertBarType == ScrollBarType::NeverShow)
						contentLayoutHeight = mClippedContentHeight;

					mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y - Math::floorToInt(mVertOffset), 
						contentLayoutWidth, contentLayoutHeight, 
						layoutClipRect, widgetDepth, areaDepth);

					mContentWidth = mContentLayout->_getActualWidth();
					mContentHeight = mContentLayout->_getActualHeight();

					hasHorzScrollbar = true;
				}
			}
		}

		// Add/remove/update vertical scrollbar as needed
		if((mVertBarType == ScrollBarType::ShowIfDoesntFit && mContentHeight > mClippedContentHeight) || mVertBarType == ScrollBarType::AlwaysShow &&
			mVertBarType != ScrollBarType::NeverShow)
		{
			if(mVertScroll == nullptr)
			{
				mVertScroll = GUIScrollBarVert::create(mScrollBarStyle);

				_registerChildElement(mVertScroll);

				mVertScroll->onScrollPositionChanged.connect(std::bind(&GUIScrollArea::vertScrollUpdate, this, _1));
			}

			INT32 scrollBarOffset = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			UINT32 scrollBarHeight = height;
			if(hasHorzScrollbar)
				scrollBarHeight = (UINT32)std::max(0, (INT32)scrollBarHeight - (INT32)ScrollBarWidth);

			Vector2I offset(x + scrollBarOffset, y);
			mVertScroll->_setOffset(offset);
			mVertScroll->_setWidth(ScrollBarWidth);
			mVertScroll->_setHeight(scrollBarHeight);
			mVertScroll->_setAreaDepth(areaDepth);
			mVertScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarWidth = std::min(width, ScrollBarWidth);
			RectI elemClipRect(0, 0, clippedScrollbarWidth, clipRect.height);
			mVertScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			RectI scrollBarLayoutClipRect(clipRect.x + scrollBarOffset, clipRect.y, clippedScrollbarWidth, clipRect.height);
			mVertScroll->_updateLayout(offset.x, offset.y, ScrollBarWidth, scrollBarHeight, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mVertScroll->getMaxHandleSize() * (scrollBarHeight / (float)mContentHeight));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentHeight) - INT32(scrollBarHeight));
			float newScrollPct = 0.0f;

			if(scrollableHeight > 0)
				newScrollPct = mVertOffset / scrollableHeight;

			mVertScroll->setHandleSize(newHandleSize);
			mVertScroll->setScrollPos(newScrollPct);
		}
		else
		{
			if(mVertScroll != nullptr)
			{
				GUIElement::destroy(mVertScroll);
				mVertScroll = nullptr;
			}

			mVertOffset = 0.0f;
		}

		// Add/remove/update horizontal scrollbar as needed
		if((mHorzBarType == ScrollBarType::ShowIfDoesntFit && mContentWidth > mClippedContentWidth) || mHorzBarType == ScrollBarType::AlwaysShow &&
			mHorzBarType != ScrollBarType::NeverShow)
		{ 
			if(mHorzScroll == nullptr)
			{
				mHorzScroll = GUIScrollBarHorz::create(mScrollBarStyle);

				_registerChildElement(mHorzScroll);

				mHorzScroll->onScrollPositionChanged.connect(std::bind(&GUIScrollArea::horzScrollUpdate, this, _1));
			}

			INT32 scrollBarOffset = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
			UINT32 scrollBarWidth = width;
			if(hasVertScrollbar)
				scrollBarWidth = (UINT32)std::max(0, (INT32)scrollBarWidth - (INT32)ScrollBarWidth);

			Vector2I offset(x, y + scrollBarOffset);
			mHorzScroll->_setOffset(offset);
			mHorzScroll->_setWidth(scrollBarWidth);
			mHorzScroll->_setHeight(ScrollBarWidth);
			mHorzScroll->_setAreaDepth(areaDepth);
			mHorzScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarHeight = std::min(height, ScrollBarWidth);
			RectI elemClipRect(0, 0, clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			RectI scrollBarLayoutClipRect(clipRect.x, clipRect.y + scrollBarOffset, clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_updateLayout(offset.x, offset.y, scrollBarWidth, ScrollBarWidth, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mHorzScroll->getMaxHandleSize() * (scrollBarWidth / (float)mContentWidth));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableWidth = (UINT32)std::max(0, INT32(mContentWidth) - INT32(scrollBarWidth));
			float newScrollPct = 0.0f;
			
			if(scrollableWidth > 0)
				newScrollPct = mHorzOffset / scrollableWidth;

			mHorzScroll->setHandleSize(newHandleSize);
			mHorzScroll->setScrollPos(newScrollPct);
		}
		else
		{
			if(mHorzScroll != nullptr)
			{
				GUIElement::destroy(mHorzScroll);
				mHorzScroll = nullptr;
			}

			mHorzOffset = 0.0f;
		}
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
		UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentHeight) - INT32(mClippedContentHeight));
		mVertOffset = scrollableHeight * Math::clamp01(pct);

		markContentAsDirty();
	}

	void GUIScrollArea::scrollToHorizontal(float pct)
	{
		UINT32 scrollableWidth = (UINT32)std::max(0, INT32(mContentWidth) - INT32(mClippedContentWidth));
		mHorzOffset = scrollableWidth * Math::clamp01(pct);

		markContentAsDirty();
	}

	void GUIScrollArea::scrollUpPx(UINT32 pixels)
	{
		if(mVertScroll != nullptr)
		{
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentHeight) - INT32(mClippedContentHeight));

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
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentHeight) - INT32(mClippedContentHeight));

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
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentWidth) - INT32(mClippedContentWidth));

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
			UINT32 scrollableSize = (UINT32)std::max(0, INT32(mContentWidth) - INT32(mClippedContentWidth));

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

	bool GUIScrollArea::mouseEvent(const GUIMouseEvent& ev)
	{
		if(ev.getType() == GUIMouseEventType::MouseWheelScroll)
		{
			// Mouse wheel only scrolls on the Y axis
			if(mVertScroll != nullptr)
			{
				UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentWidth) - INT32(mClippedContentHeight));
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