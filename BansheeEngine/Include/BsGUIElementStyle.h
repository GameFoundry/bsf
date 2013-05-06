#pragma once

#include "BsPrerequisites.h"
#include "CmColor.h"

namespace BansheeEngine
{
	struct RectOffset
	{
		RectOffset()
			:left(0), right(0), top(0), bottom(0)
		{ }

		INT32 left, right, top, bottom;
	};

	struct BS_EXPORT GUIElementStyle
	{
		struct BS_EXPORT GUIElementStateStyle
		{
			SpriteTexturePtr texture;
			CM::Color textColor;
		};

		GUIElementStyle()
			:fontSize(8)
		{

		}

		CM::HFont font;
		UINT32 fontSize;

		GUIElementStateStyle normal;
		GUIElementStateStyle hover;
		GUIElementStateStyle active;
		GUIElementStateStyle focused;

		// For controls that can be turned on-off
		GUIElementStateStyle normalOn;
		GUIElementStateStyle hoverOn;
		GUIElementStateStyle activeOn;
		GUIElementStateStyle focusedOn;

		RectOffset border;
	};
}