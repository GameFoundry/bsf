#include "BsGUIArea.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutX.h"
#include "CmViewport.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIArea::GUIArea(GUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT16 depth)
		:mWidget(widget), mX(x), mY(y), mWidth(width), mHeight(height), mDepth(depth), mIsDirty(true), 
		mResizeHeightWithWindow(false), mResizeWidthWithWindow(false)
	{
		mLayout = cm_new<GUILayoutX, PoolAlloc>();

		if(width <= 0)
		{
			mWidth = mWidget.getTarget()->getWidth();
			mResizeWidthWithWindow = true;
		}

		if(height <= 0)
		{
			mHeight = mWidget.getTarget()->getHeight();
			mResizeHeightWithWindow = true;
		}

		mWidget.registerArea(this);
	}

	GUIArea::~GUIArea() 
	{
		cm_delete<PoolAlloc>(mLayout);
	}

	GUIArea* GUIArea::create(GUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT16 depth)
	{
		return new (cm_alloc<GUIArea, PoolAlloc>()) GUIArea(widget, x, y, width, height, depth);
	}

	void GUIArea::destroy(GUIArea* area)
	{
		area->mWidget.unregisterArea(area);

		cm_delete<PoolAlloc>(area);
	}

	void GUIArea::destroyInternal(GUIArea* area)
	{
		cm_delete<PoolAlloc>(area);
	}

	void GUIArea::_update()
	{
		if(isDirty())
		{
			mLayout->_update(mX, mY, mWidth, mHeight, mWidget.getDepth(), mDepth);
			mIsDirty = false;
		}
	}

	bool GUIArea::isDirty() const
	{
		if(mIsDirty)
			return true;

		return mLayout->_isDirty();
	}

	void GUIArea::notifyWindowResized(UINT32 newWidth, UINT32 newHeight)
	{
		if(mResizeWidthWithWindow)
			mWidth = newWidth;

		if(mResizeHeightWithWindow)
			mHeight = newHeight;

		if(mResizeWidthWithWindow || mResizeHeightWithWindow)
		{
			mIsDirty = true;
		}
	}
}