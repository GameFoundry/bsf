//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	/** @addtogroup Utility-Core-Internal
	 *  @{
	 */

	/**	Manipulates icons in executable files. */
	class BS_CORE_EXPORT IconUtility
	{
	public:
		/**
		 * Updates icons in the provided executable. Only icons that already exist in the executable can be updated, no new
		 * icons can be inserted. Icons must be square.
		 *
		 * @param[in]	filePath	Path to the executable.
		 * @param[in]	icons   	Pixels of images to replace. Each entry maps an icon width (and height, since they're
		 *							square) to its pixels.
		 */
		static void updateIconExe(const Path& filePath, const Map<UINT32, SPtr<PixelData>>& icons);

	private:
		/**
		 * Updates data of an existing icon with new pixels.
		 *
		 * @param[in] 	iconData	Existing icon bytes, in Windows ICO format.
		 * @param[in]	icons		Pixels of images to replace. Each entry maps an icon width (and height, since they're
		 *							square) to its pixels.
		 */
		static void updateIconData(UINT8* iconData, const Map<UINT32, SPtr<PixelData>>& icons);
	};

	/** @} */
}
