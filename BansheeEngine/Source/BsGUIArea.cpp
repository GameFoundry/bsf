#include "BsGUIArea.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutX.h"
#include "CmViewport.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIArea::GUIArea(GUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT16 depth)
		:mWidget(widget), mX(x), mY(y), mWidth(width), mHeight(height), mDepth(depth), mIsDirty(true)
	{
		mLayout = CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX();

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
		CM_DELETE(mLayout, GUILayout, PoolAlloc);
	}

	GUIArea* GUIArea::create(GUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT16 depth)
	{
		return CM_NEW(GUIArea, PoolAlloc) GUIArea(widget, x, y, width, height, depth);
	}

	void GUIArea::destroy(GUIArea* area)
	{
		area->mWidget.unregisterArea(area);

		CM_DELETE(area, GUIArea, PoolAlloc);
	}

	void GUIArea::destroyInternal(GUIArea* area)
	{
		CM_DELETE(area, GUIArea, PoolAlloc);
	}

	void GUIArea::_update()
	{
		if(isDirty())
		{
			UINT32 combinedDepth = UINT32(mWidget.getDepth()) << 16 | UINT32(mDepth);

			mLayout->_update(mX, mY, mWidth, mHeight, combinedDepth);
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