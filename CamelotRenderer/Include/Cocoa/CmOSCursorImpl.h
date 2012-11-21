#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmInt2.h"

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#else
#include <ApplicationServices/ApplicationServices.h>
typedef void *id;
#endif

namespace CamelotEngine
{
	/**
	 * @brief	Provides controls for Windows operating system cursor.
	 */
	class CM_UTILITY_EXPORT OSCursor
	{
	public:
		static Int2 getPosition();
		static void setPosition(const Int2& pos);

		static void hide();
		static void show();
	};
}