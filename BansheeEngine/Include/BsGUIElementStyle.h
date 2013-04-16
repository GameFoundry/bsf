#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	struct BS_EXPORT GUIElementStyle
	{
		GUIElementStyle()
			:fontSize(8)
		{

		}

		CM::HFont font;
		UINT32 fontSize;
	};
}