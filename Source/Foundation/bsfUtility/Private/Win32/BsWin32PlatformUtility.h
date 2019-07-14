//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once
#include "Prerequisites/BsPrerequisitesUtil.h"
#include <windows.h>

namespace bs
{
	/** @addtogroup Internal-Utility
	 *  @{
	 */

	/** @addtogroup Platform-Utility-Internal
	 *  @{
	 */

	/** Provides access to various Windows specific utility functions. */
	class BS_UTILITY_EXPORT Win32PlatformUtility
	{
	public:
		/**
		 * Creates a new bitmap usable by various Win32 methods from the provided pixel data. Caller must ensure to call
		 * DeleteObject() on the bitmap handle when finished.
		 */
		static HBITMAP createBitmap(const Color* pixels, UINT32 width, UINT32 height, bool premultiplyAlpha);
	};

	/** @} */
	/** @} */
}
