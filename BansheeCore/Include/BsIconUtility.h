#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manipulates icons in executable files.
	 */
	class BS_CORE_EXPORT IconUtility
	{
	public:
		/**
		 * @brief	Updates icons in the provided executable. Only icons that already exist in the executable can be
		 * 			updated, no new icons can be inserted. Icons must be square.
		 *
		 * @param	filePath	Path to the executable.
		 * @param	icons   	Pixels of images to replace. Each entry maps an icon width (and height, since 
		 * 						they're square) to its pixels.
		 */
		static void updateIconExe(const Path& filePath, const Map<UINT32, PixelDataPtr>& icons);

	private:
		/**
		 * @brief	Updates data of an existing icon with new pixels.
		 *
		 * @param 	iconData	Existing icon bytes, in Windows ICO format.
		 * @param	icons		Pixels of images to replace. Each entry maps an icon width (and height, since 
		 * 						they're square) to its pixels.
		 */
		static void updateIconData(UINT8* iconData, const Map<UINT32, PixelDataPtr>& icons);
	};
}