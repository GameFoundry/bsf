#include "BsGUIArea.h"
#include "BsGUIWidget.h"
#include "BsGUILayoutX.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIArea::GUIArea(GUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT16 depth)
		:mWidget(widget), mX(x), mY(y), mWidth(width), mHeight(height), mDepth(depth)
	{
		mLayout = CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX();

		resizeWidthWithWindow = width == 0;
		resizeHeightWithWindow = height == 0;

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

	void GUIArea::notifyWindowResized(UINT32 newWidth, UINT32 newHeight)
	{
		if(resizeWidthWithWindow)
			mWidth = newWidth;

		if(resizeHeightWithWindow)
			mHeight = newHeight;

		if(resizeWidthWithWindow || resizeHeightWithWindow)
		{
			UINT32 combinedDepth = UINT32(mWidget.getDepth()) << 16 | UINT32(mDepth);

			mLayout->_update(mX, mY, mWidth, mHeight, combinedDepth);
		}
	}
}