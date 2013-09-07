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

	GUIScrollArea::GUIScrollArea(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions, false), mVertScroll(nullptr), mHorzScroll(nullptr), mVertOffset(0), mHorzOffset(0),
		mContentWidth(0), mContentHeight(0), mClippedContentWidth(0), mClippedContentHeight(0)
	{
		mContentLayout = &addLayoutYInternal(this);
	}

	GUIScrollArea::~GUIScrollArea()
	{

	}

	UINT32 GUIScrollArea::getNumRenderElements() const
	{
		return 0;
	}

	const HMaterial& GUIScrollArea::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InternalErrorException, "Trying to retrieve a material from an element with no render elements.");
	}

	UINT32 GUIScrollArea::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIScrollArea::updateRenderElementsInternal()
	{
		GUIElement::updateRenderElementsInternal();
	}

	void GUIScrollArea::updateClippedBounds()
	{
		mClippedBounds = Rect(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	UINT32 GUIScrollArea::_getOptimalWidth() const
	{
		return mContentLayout->_getOptimalWidth();
	}

	UINT32 GUIScrollArea::_getOptimalHeight() const
	{
		return mContentLayout->_getOptimalHeight();
	}

	void GUIScrollArea::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	void GUIScrollArea::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mContentLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
		UINT32 contentWidth = mContentLayout->_getActualWidth();
		UINT32 contentHeight = mContentLayout->_getActualHeight();

		mClippedContentWidth = width;
		mClippedContentHeight = height;

		bool hasScrollbars = false;
		Rect layoutClipRect = clipRect;

		bool hasHorzScrollbar = false;
		if(contentWidth > mWidth)
		{ 
			// Make room for scrollbar
			mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
			layoutClipRect.height = mClippedContentHeight;
			hasHorzScrollbar = true;
		}

		if(contentHeight > mClippedContentHeight)
		{
			// Make room for scrollbar
			mClippedContentWidth = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);
			layoutClipRect.width = mClippedContentWidth;

			if(!hasHorzScrollbar && contentWidth > mClippedContentWidth) // Since width has been reduced, we need to check if we require the horizontal scrollbar
			{
				// Make room for scrollbar
				mClippedContentHeight = (UINT32)std::max(0, (INT32)height - (INT32)ScrollBarWidth);
				layoutClipRect.height = mClippedContentHeight;
			}
		}

		// Add/remove/update vertical scrollbar as needed
		if(contentHeight > mClippedContentHeight)
		{
			if(mVertScroll == nullptr)
			{
				mVertScroll = GUIScrollBarVert::create(_getParentWidget());
				mVertScroll->onScrollPositionChanged.connect(boost::bind(&GUIScrollArea::vertScrollUpdate, this, _1));
				mVertScroll->_setAcceptsKeyboardFocus(false);
			}

			Int2 offset(x + mClippedContentWidth, y);
			mVertScroll->_setOffset(offset);
			mVertScroll->_setWidth(ScrollBarWidth);
			mVertScroll->_setHeight(mClippedContentHeight);
			mVertScroll->_setAreaDepth(areaDepth);
			mVertScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarWidth = std::min(width, ScrollBarWidth);
			Rect elemClipRect(0, 0, clippedScrollbarWidth, clipRect.height);
			mVertScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			Rect scrollBarLayoutClipRect(clipRect.x + mClippedContentWidth, clipRect.y, clippedScrollbarWidth, clipRect.height);
			mVertScroll->_updateLayout(offset.x, offset.y, ScrollBarWidth, mClippedContentHeight, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::FloorToInt(mVertScroll->getMaxHandleSize() * (mClippedContentHeight / (float)contentHeight));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableHeight = (UINT32)std::max(0, INT32(contentHeight) - INT32(mClippedContentHeight));
			float newScrollPct = mVertOffset / scrollableHeight;

			mVertScroll->setHandleSize(newHandleSize);
			mVertScroll->setScrollPos(newScrollPct);

			hasScrollbars = true;
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
		if(contentWidth > mClippedContentWidth)
		{ 
			if(mHorzScroll == nullptr)
			{
				mHorzScroll = GUIScrollBarHorz::create(_getParentWidget());
				mHorzScroll->onScrollPositionChanged.connect(boost::bind(&GUIScrollArea::horzScrollUpdate, this, _1));
				mHorzScroll->_setAcceptsKeyboardFocus(false);
			}

			Int2 offset(x, y + mClippedContentHeight);
			mHorzScroll->_setOffset(offset);
			mHorzScroll->_setWidth(mClippedContentWidth);
			mHorzScroll->_setHeight(ScrollBarWidth);
			mHorzScroll->_setAreaDepth(areaDepth);
			mHorzScroll->_setWidgetDepth(widgetDepth);

			UINT32 clippedScrollbarHeight = std::min(height, ScrollBarWidth);
			Rect elemClipRect(0, 0, clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_setClipRect(elemClipRect);

			// This element is not a child of any layout so we treat it as a root element
			Rect scrollBarLayoutClipRect(clipRect.x, clipRect.y + mClippedContentHeight, clipRect.width, clippedScrollbarHeight);
			mHorzScroll->_updateLayout(offset.x, offset.y, mClippedContentWidth, ScrollBarWidth, scrollBarLayoutClipRect, widgetDepth, areaDepth);

			// Set new handle size and update position to match the new size
			UINT32 newHandleSize = (UINT32)Math::FloorToInt(mHorzScroll->getMaxHandleSize() * (mClippedContentWidth / (float)contentWidth));
			newHandleSize = std::max(newHandleSize, MinHandleSize);

			UINT32 scrollableWidth = (UINT32)std::max(0, INT32(contentWidth) - INT32(mClippedContentWidth));
			float newScrollPct = mHorzOffset / scrollableWidth;

			mHorzScroll->setHandleSize(newHandleSize);
			mHorzScroll->setScrollPos(newScrollPct);

			hasScrollbars = true;
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

		if(hasScrollbars)
		{
			mContentLayout->_updateLayoutInternal(x - Math::FloorToInt(mHorzOffset), y - Math::FloorToInt(mVertOffset), 
				mClippedContentWidth + Math::FloorToInt(mHorzOffset), mClippedContentHeight + Math::FloorToInt(mVertOffset), 
				layoutClipRect, widgetDepth, areaDepth);
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

	void GUIScrollArea::_changeParentWidget(GUIWidget* widget)
	{
		GUIElement::_changeParentWidget(widget);

		// These two elements are not part of a layout so I need to make sure to
		// update them manually
		if(mVertScroll != nullptr)
			mVertScroll->_changeParentWidget(widget);

		if(mHorzScroll != nullptr)
			mHorzScroll->_changeParentWidget(widget);
	}

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, style, getDefaultLayoutOptions(style));
	}

	GUIScrollArea* GUIScrollArea::create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin* skin = parent.getSkin();
			style = skin->getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUIScrollArea, PoolAlloc>()) GUIScrollArea(parent, style, layoutOptions);
	}

	const String& GUIScrollArea::getGUITypeName()
	{
		static String typeName = "ScrollArea";
		return typeName;
	}
}