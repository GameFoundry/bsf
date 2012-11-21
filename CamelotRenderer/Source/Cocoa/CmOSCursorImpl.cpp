#include "X11/CmOSCursorImpl.h"

// TODO - Not tested and will not work until I properly test it on a mac system!

namespace CamelotEngine
{
	OSCursor()
	{
	}

	Int2 OSCursor::getPosition()
	{
		NSPoint mouseLoc;
		mouseLoc = [NSEvent mouseLocation];

		return Int2(mouseLoc.x, mouseLoc.y);
	}

	void OSCursor::setPosition(const Int2& pos)
	{
		CGPoint globalPoint = CGPointMake(pos.x, pos.y);
		CGWarpMouseCursorPosition(globalPoint);
	}

	void OSCursor::hide()
	{
		[NSCursor hide];
		CGAssociateMouseAndMouseCursorPosition(false);
	}

	void OSCursor::show()
	{
		[NSCursor unhide];
		CGAssociateMouseAndMouseCursorPosition(true);
	}
}