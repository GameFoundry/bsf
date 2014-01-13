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
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const UINT32 GUIScrollArea::ScrollBarWidth = 8;
	const UINT32 GUIScrollArea::MinHandleSize = 4;
	const UINT32 GUIScrollArea::WheelScrollAmount = 50;

	GUIScrollArea::GUIScrollArea(GUIWidget& parent, ScrollBarType vertBarType, ScrollBarType horzBarType, 
		const GUIElementStyle* scrollBarStyle, const GUIElementStyle* scrollAreaStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mVertScroll(nullptr), mHorzScroll(nullptr), mVertOffset(0), mHorzOffset(0),
		mContentWidth(0), mContentHeight(0), mClippedContentWidth(0), mClippedContentHeight(0), mVertBarType(vertBarType), mHorzBarType(horzBarType),
		mScrollBarStyle(scrollBarStyle)
	{
		mContentLayout = &addLayoutYInternal(this);
	}

	GUIScrollArea::~GUIScrollArea()
	{

	}

	void GUIScrollArea::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mContentLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
		UINT32 contentWidth = mContentLayout->_getActualWidth();
		UINT32 contentHeight = mContentLayout->_getActualHeight();

		mClippedContentWidth = width;
		mClippedContentHeight = height;

		RectI layoutClipRect = clipRect;
		bool addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && contentWidth > mWidth) || 
			mHorzBarType == ScrollBarType::AlwaysShow && mHorzBarType != ScrollBarType::NeverShow;

		bool hasHorzScrollbar = false;
		bool hasVertScrollbar = false;
		if(addHorzScrollbar)
		{ 
			// Make room for scrollbar
			mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
			layoutClipRect.height = mClippedContentHeight;
			hasHorzScrollbar = true;

			mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y, 
				mClippedContentWidth + Math::floorToInt(mHorzOffset), mClippedContentHeight, layoutClipRect, widgetDepth, areaDepth);

			contentWidth = mContentLayout->_getActualWidth();
			contentHeight = mContentLayout->_getActualHeight();
		}

		bool addVertScrollbar = (mVertBarType == ScrollBarType::ShowIfDoesntFit && contentHeight > mClippedContentHeight) || 
			mVertBarType == ScrollBarType::AlwaysShow && mVertBarType != ScrollBarType::NeverShow;

		if(addVertScrollbar)
		{
			// Make room for scrollbar
			mClippedContentWidth = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			layoutClipRect.width = mClippedContentWidth;
			hasVertScrollbar = true;

			if(hasHorzScrollbar)
			{
				mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y - Math::floorToInt(mVertOffset), 
					mClippedContentWidth + Math::floorToInt(mHorzOffset), mClippedContentHeight + Math::floorToInt(mVertOffset), 
					layoutClipRect, widgetDepth, areaDepth);
			}
			else
			{
				mContentLayout->_updateLayoutInternal(x, y - Math::floorToInt(mVertOffset), 
					mClippedContentWidth, mClippedContentHeight + Math::floorToInt(mVertOffset), 
					layoutClipRect, widgetDepth, areaDepth);
			}

			contentWidth = mContentLayout->_getActualWidth();
			contentHeight = mContentLayout->_getActualHeight();

			if(!hasHorzScrollbar) // Since width has been reduced, we need to check if we require the horizontal scrollbar
			{
				addHorzScrollbar = (mHorzBarType == ScrollBarType::ShowIfDoesntFit && contentWidth > mClippedContentWidth) && mHorzBarType != ScrollBarType::NeverShow;

				if(addHorzScrollbar)
				{
					// Make room for scrollbar
					mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
					layoutClipRect.height = mClippedContentHeight;

					mContentLayout->_updateLayoutInternal(x - Math::floorToInt(mHorzOffset), y - Math::floorToInt(mVertOffset), 
						mClippedContentWidth + Math::floorToInt(mHorzOffset), mClippedContentHeight + Math::floorToInt(mVertOffset), 
						layoutClipRect, widgetDepth, areaDepth);
				}
			}
		}

		// Add/remove/update vertical scrollbar as needed
		if((mVertBarType == ScrollBarType::ShowIfDoesntFit && contentHeight > mClippedContentHeight) || mVertBarType == ScrollBarType::AlwaysShow &&
			mVertBarType != ScrollBarType::NeverShow)
		{
			if(mVertScroll == nullptr)
			{
				if(mScrollBarStyle != nullptr)
					mVertScroll = GUIScrollBarVert::create(_getParentWidget(), mScrollBarStyle);
				else
					mVertScroll = GUIScrollBarVert::create(_getParentWidget());

				_registerChildElement(mVertScroll);

				mVertScroll->onScrollPositionChanged.connect(boost::bind(&GUIScrollArea::vertScrollUpdate, this, _1));
				mVertScroll->_setAcceptsKeyboardFocus(false);
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
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mVertScroll->getMaxHandleSize() * (scrollBarHeight / (float)contentHeight));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableHeight = (UINT32)std::max(0, INT32(contentHeight) - INT32(scrollBarHeight));
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
		if((mHorzBarType == ScrollBarType::ShowIfDoesntFit && contentWidth > mClippedContentWidth) || mHorzBarType == ScrollBarType::AlwaysShow &&
			mHorzBarType != ScrollBarType::NeverShow)
		{ 
			if(mHorzScroll == nullptr)
			{
				if(mScrollBarStyle != nullptr)
					mHorzScroll = GUIScrollBarHorz::create(_getParentWidget(), mScrollBarStyle);
				else
					mHorzScroll = GUIScrollBarHorz::create(_getParentWidget());

				_registerChildElement(mHorzScroll);

				mHorzScroll->onScrollPositionChanged.connect(boost::bind(&GUIScrollArea::horzScrollUpdate, this, _1));
				mHorzScroll->_setAcceptsKeyboardFocus(false);
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
			UINT32 newHandleSize = (UINT32)Math::floorToInt(mHorzScroll->getMaxHandleSize() * (scrollBarWidth / (float)contentWidth));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableWidth = (UINT32)std::max(0, INT32(contentWidth) - INT32(scrollBarWidth));
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

		mContentWidth = contentWidth;
		mContentHeight = contentHeight;
	}

	void GUIScrollArea::vertScrollUpdate(float scrollPos)
	{
		UINT32 scrollableHeight = (UINT32)std::max(0, INT32(mContentHeight) - INT32(mClippedContentHeight));
		mVertOffset = scrollableHeight * scrollPos;

		markContentAsDirty();
	}

	void GUIScrollArea::horzScrollUpdate(float scrollPos)
	{
		UINT32 scrollableWidth = (UINT32)std::max(0, INT32(mContentWidth) - INT32(mClippedContentWidth));
		mHorzOffset = scrollableWidth * scrollPos;

		markContentAsDirty();
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

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, ScrollBarType vertBarType, ScrollBarType horzBarType, 
		const GUIElementStyle* scrollBarStyle, const GUIElementStyle* scrollAreaStyle)
	{
		if(scrollAreaStyle == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			scrollAreaStyle = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, vertBarType, horzBarType, scrollBarStyle, 
			scrollAreaStyle, GUILayoutOptions::create(scrollAreaStyle));
	}

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* scrollBarStyle, 
		const GUIElementStyle* scrollAreaStyle)
	{
		if(scrollAreaStyle == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			scrollAreaStyle = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, ScrollBarType::ShowIfDoesntFit, 
			ScrollBarType::ShowIfDoesntFit, scrollBarStyle, scrollAreaStyle, GUILayoutOptions::create(layoutOptions, scrollAreaStyle));
	}

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, const GUIElementStyle* scrollBarStyle, const GUIElementStyle* scrollAreaStyle)
	{
		if(scrollAreaStyle == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			scrollAreaStyle = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, ScrollBarType::ShowIfDoesntFit, ScrollBarType::ShowIfDoesntFit, scrollBarStyle, 
			scrollAreaStyle, GUILayoutOptions::create(scrollAreaStyle));
	}

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, ScrollBarType vertBarType, 
		ScrollBarType horzBarType, const GUIOptions& layoutOptions, const GUIElementStyle* scrollBarStyle, 
		const GUIElementStyle* scrollAreaStyle)
	{
		if(scrollAreaStyle == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			scrollAreaStyle = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, vertBarType, horzBarType, scrollBarStyle, scrollAreaStyle, GUILayoutOptions::create(layoutOptions, scrollAreaStyle));
	}

	const String& GUIScrollArea::getGUITypeName()
	{
		static String typeName = "ScrollArea";
		return typeName;
	}
}