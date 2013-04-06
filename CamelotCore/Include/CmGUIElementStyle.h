#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	struct CM_EXPORT GUIElementStyle
	{
		GUIElementStyle()
			:fontSize(8)
		{

		}

		HFont font;
		UINT32 fontSize;
	};
}