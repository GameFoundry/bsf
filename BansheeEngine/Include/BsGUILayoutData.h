#pragma once

#include "BsPrerequisites.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains all attributes that are output by GUI layouts and
	 *			assigned to GUI elements. This includes element position,
	 *			size and depth.
	 */
	struct BS_EXPORT GUILayoutData
	{
		GUILayoutData()
			:depthRangeMin(-1), depthRangeMax(-1), depth(0)
		{ 
			setPanelDepth(0);
		}

		void setWidgetDepth(UINT8 depth)
		{
			UINT32 shiftedDepth = depth << 24;

			depth = shiftedDepth | (depth & 0x00FFFFFF);
		}

		void setPanelDepth(INT16 depth)
		{
			UINT32 signedDepth = ((INT32)depth + 32768) << 8;

			depth = signedDepth | (depth & 0xFF0000FF);;
		}

		UINT8 getWidgetDepth() const
		{
			return (depth >> 24) & 0xFF;
		}

		INT16 getPanelDepth() const
		{
			return (((INT32)depth >> 8) & 0xFFFF) - 32768;
		}

		Rect2I area;
		Rect2I clipRect;
		UINT32 depth;
		UINT16 depthRangeMin;
		UINT16 depthRangeMax;
	};
}