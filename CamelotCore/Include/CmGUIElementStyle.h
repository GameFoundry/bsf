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

		FontPtr font;
		UINT32 fontSize;
	};
}