#include "BsGUIArea.h"
#include "BsGUILayoutX.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIArea::GUIArea(const HGUIWidget& widget, UINT32 x, UINT32 y, UINT32 width, UINT32 height)
		:mWidget(widget), mX(x), mY(y), mWidth(width), mHeight(height), mDepth(0)
	{
		mLayout = new GUILayoutX();
	}

	GUIArea::~GUIArea() 
	{

	}
}