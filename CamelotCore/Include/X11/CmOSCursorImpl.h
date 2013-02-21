#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmInt2.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace CamelotEngine
{
	/**
	 * @brief	Provides controls for operating system cursor.
	 */
	class CM_UTILITY_EXPORT OSCursor
	{
	public:
		OSCursor();

		static Int2 getPosition();
		static void setPosition(const Int2& pos);

		static void hide();
		static void show();

	private:
		Window mWindow; // TODO - This isn't being set anywhere yet
		Cursor mCursor;
		Display* mDisplay;

		bool mCursorGrabbed;
		bool mCursorHidden;
	};
}