#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Volume of pixels used for referencing a part of
	 * 			a texture.
	 */
	struct BS_CORE_EXPORT PixelVolume
	{
		UINT32 left, top, right, bottom, front, back;

		PixelVolume()
			: left(0), top(0), right(1), bottom(1), front(0), back(1)
		{ }

		PixelVolume(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom):
			left(left), top(top), right(right), bottom(bottom), front(0), back(1)
		{
			assert(right >= left && bottom >= top && back >= front);
		}

		PixelVolume(UINT32 left, UINT32 top, UINT32 front, UINT32 right, UINT32 bottom, UINT32 back):
			left(left), top(top), right(right), bottom(bottom), front(front), back(back)
		{
			assert(right >= left && bottom >= top && back >= front);
		}
            
		/**
		 * @brief	Return true if the other box is a part of this one.
		 */
		bool contains(const PixelVolume &volume) const
		{
			return (volume.left >= left && volume.top >= top && volume.front >= front &&
				volume.right <= right && volume.bottom <= bottom && volume.back <= back);
		}
            
		UINT32 getWidth() const { return right-left; }
		UINT32 getHeight() const { return bottom-top; }
		UINT32 getDepth() const { return back-front; }
	};
}