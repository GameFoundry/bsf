#include "BsGUIScrollArea.h"
#include "BsGUIElementStyle.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutOptions.h"
#include "BsGUILayout.h"
#include "BsGUISkin.h"
#include "BsGUIScrollBarVert.h"
#include "BsGUIScrollBarHorz.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const UINT32 GUIScrollArea::ScrollBarWidth = 6;

	GUIScrollArea::GUIScrollArea(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions)
		:GUIElement(parent, style, layoutOptions), mVertScroll(nullptr), mHorzScroll(nullptr), mVertOffset(0), mHorzOffset(0),
		mContentWidth(0), mContentHeight(0)
	{
		mContentLayout = &addLayoutYInternal();
		mVertScroll = GUIScrollBarVert::create(parent);
		
		mVertScroll->scrollPositionChanged.connect(boost::bind(&GUIScrollArea::vertScrollUpdate, this, _1));

		mHorzScroll = GUIScrollBarHorz::create(parent);
		// mHorzScroll->scrollPositionChanged.connect(boost::bind(&GUIScrollArea::horzScrollUpdate, this, _1)); // TODO - Enable
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
	{ }

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

	void GUIScrollArea::_updateLayoutInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height,
		Rect clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mContentLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
		UINT32 contentWidth = mContentLayout->_getActualWidth();
		UINT32 contentHeight = mContentLayout->_getActualHeight();

		if(contentHeight > mHeight)
		{
			// Make room for scrollbar
			UINT32 contentWidth = (UINT32)std::max(0, (INT32)width - (INT32)ScrollBarWidth);

			Rect layoutClipRect(clipRect.x, clipRect.y, contentWidth, clipRect.height);
			mContentLayout->_updateLayoutInternal(x, y - mVertOffset, contentWidth, height, layoutClipRect, widgetDepth, areaDepth);
			contentWidth = mContentLayout->_getActualWidth();
			contentHeight = mContentLayout->_getActualHeight();

			mVertScroll = GUIScrollBarVert::create(_getParentWidget());
			mVertScroll->scrollPositionChanged.connect(boost::bind(&GUIScrollArea::vertScrollUpdate, this, _1));

			Int2 offset(x + contentWidth, y);
			mVertScroll->_setOffset(offset);
			mVertScroll->_setWidth(ScrollBarWidth);
			mVertScroll->_setHeight(height);
			mVertScroll->_setAreaDepth(areaDepth);
			mVertScroll->_setWidgetDepth(widgetDepth);

			Rect elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mVertScroll->_setClipRect(elemClipRect);

			// TODO - Update scroll handle size and update position to match
		}
		else
		{
			if(mVertScroll != nullptr)
			{
				GUIElement::destroy(mVertScroll);
				mVertScroll = nullptr;
			}
		}

		// TODO - Add horizontal scroll bar
		
		mContentWidth = contentWidth;
		mContentHeight = contentHeight;
	}

	void GUIScrollArea::vertScrollUpdate(float scrollPos)
	{
		mVertOffset = Math::FloorToInt(mContentHeight * scrollPos);
	}

	void GUIScrollArea::horzScrollUpdate(float scrollPos)
	{
		mHorzOffset = Math::FloorToInt(mContentWidth * scrollPos);
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