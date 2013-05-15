#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	struct GUI_LAYOUT_OPTIONS
	{
		GUI_LAYOUT_OPTIONS()
			:width(0), height(0), minWidth(0), maxWidth(0),
			minHeight(0), maxHeight(0), fixedWidth(false), fixedHeight(false)
		{

		}

		GUI_LAYOUT_OPTIONS(UINT32 _width, UINT32 _height)
			:width(_width), height(_height), minWidth(0), maxWidth(0),
			minHeight(0), maxHeight(0), fixedWidth(true), fixedHeight(true)
		{

		}

		UINT32 width, height;
		UINT32 minWidth, maxWidth, minHeight, maxHeight;
		bool fixedWidth, fixedHeight;
	};
}