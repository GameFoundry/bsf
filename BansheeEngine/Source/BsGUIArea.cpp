//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUIArea.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutX.h"
#include "BsGUIWidget.h"
#include "BsRenderWindow.h"
#include "BsViewport.h"

namespace BansheeEngine
{
	GUIArea::GUIArea(GUIWidget* widget, INT32 x, INT32 y, UINT16 depth)
		:mWidget(widget), mLeft(x), mTop(y), mDepth(depth), mIsDirty(true), mIsDisabled(false),
		mResizeXWithWidget(false), mResizeYWithWidget(false), mWidth(0), mHeight(0), mRight(0), mBottom(0)
	{
		mLayout = bs_new<GUILayoutX, PoolAlloc>();
		mLayout->_changeParentWidget(widget);

		mWidget->registerArea(this);
	}

	GUIArea::~GUIArea() 
	{
		bs_delete<PoolAlloc>(mLayout);
	}

	GUIArea* GUIArea::create(GUIWidget& widget, INT32 x, INT32 y, UINT32 width, UINT32 height, UINT16 depth)
	{
		GUIArea* area = new (bs_alloc<GUIArea, PoolAlloc>()) GUIArea(&widget, x, y, depth);
		area->mWidth = width;
		area->mHeight = height;

		return area;
	}

	GUIArea* GUIArea::createStretchedXY(GUIWidget& widget, UINT32 offsetLeft, 
		UINT32 offsetRight, UINT32 offsetTop, UINT32 offsetBottom, UINT16 depth)
	{
		GUIArea* area = new (bs_alloc<GUIArea, PoolAlloc>()) GUIArea(&widget, offsetLeft, offsetTop, depth);

		area->mWidth = std::max(0, (INT32)widget.getTarget()->getWidth() - (INT32)offsetLeft - (INT32)offsetRight);
		area->mHeight = std::max(0, (INT32)widget.getTarget()->getHeight() - (INT32)offsetTop - (INT32)offsetBottom);
		area->mRight = offsetRight;
		area->mBottom = offsetBottom;
		area->mResizeXWithWidget = true;
		area->mResizeYWithWidget = true;

		return area;
	}

	GUIArea* GUIArea::createStretchedX(GUIWidget& widget, UINT32 offsetLeft, 
		UINT32 offsetRight, UINT32 offsetTop, UINT32 height, UINT16 depth)
	{
		GUIArea* area = new (bs_alloc<GUIArea, PoolAlloc>()) GUIArea(&widget, offsetLeft, offsetTop, depth);

		area->mWidth = std::max(0, (INT32)widget.getTarget()->getWidth() - (INT32)offsetLeft - (INT32)offsetRight);
		area->mHeight = height;
		area->mRight = offsetRight;
		area->mResizeXWithWidget = true;
		area->mResizeYWithWidget = false;

		return area;
	}

	GUIArea* GUIArea::createStretchedY(GUIWidget& widget, UINT32 offsetTop, 
		UINT32 offsetBottom, UINT32 offsetLeft, UINT32 width, UINT16 depth)
	{
		GUIArea* area = new (bs_alloc<GUIArea, PoolAlloc>()) GUIArea(&widget, offsetLeft, offsetTop, depth);

		area->mWidth = width;
		area->mHeight = std::max(0, (INT32)widget.getTarget()->getHeight() - (INT32)offsetTop - (INT32)offsetBottom);
		area->mBottom = offsetBottom;
		area->mResizeXWithWidget = false;
		area->mResizeYWithWidget = true;

		return area;
	}

	void GUIArea::destroy(GUIArea* area)
	{
		if(area->mWidget != nullptr)
			area->mWidget->unregisterArea(area);

		bs_delete<PoolAlloc>(area);
	}

	void GUIArea::destroyInternal(GUIArea* area)
	{
		bs_delete<PoolAlloc>(area);
	}

	void GUIArea::disable()
	{
		mIsDisabled = true;

		mLayout->disableRecursively();
	}

	void GUIArea::enable()
	{
		mIsDisabled = false;

		mLayout->enableRecursively();
	}

	void GUIArea::changeParentWidget(GUIWidget* widget)
	{
		if(mWidget == widget)
			return;

		if(mWidget != nullptr)
			mWidget->unregisterArea(this);

		if(widget != nullptr)
			widget->registerArea(this);

		mWidget = widget;

		mLayout->_changeParentWidget(widget);

		if(mWidget != nullptr)
		{
			// Ensure the size is valid, otherwise next GUI layout update will calculate wrong element coordinates
			updateSizeBasedOnParent(mWidget->getTarget()->getWidth(), mWidget->getTarget()->getHeight());
		}

		mIsDirty = true;
	}

	void GUIArea::_update()
	{
		if(!mIsDisabled && isDirty() && (mWidget != nullptr))
		{
			RectI clipRect(mLeft, mTop, mWidth, mHeight);
			mLayout->_updateLayout(mLeft, mTop, mWidth, mHeight, clipRect, mWidget->getDepth(), mDepth);
			mIsDirty = false;
		}
	}

	bool GUIArea::isDirty() const
	{
		if(mIsDirty)
			return true;

		return mLayout->_isContentDirty();
	}

	void GUIArea::setPosition(INT32 x, INT32 y)
	{
		mLeft = x;
		mTop = y;

		mIsDirty = true;
	}

	void GUIArea::setSize(UINT32 width, UINT32 height)
	{
		mWidth = width;
		mHeight = height;

		mIsDirty = true;
	}

	void GUIArea::updateSizeBasedOnParent(UINT32 parentWidth, UINT32 parentHeight)
	{
		if(mResizeXWithWidget)
			mWidth = (UINT32)std::max(0, (INT32)parentWidth - (INT32)mLeft - (INT32)mRight);

		if(mResizeYWithWidget)
			mHeight = (UINT32)std::max(0, (INT32)parentHeight - (INT32)mTop - (INT32)mBottom);

		if(mResizeXWithWidget || mResizeYWithWidget)
			mIsDirty = true;
	}
}