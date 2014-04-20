#pragma once

#include "BsPrerequisites.h"
#include "BsRectOffset.h"
#include "BsTextSprite.h"
#include "CmColor.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	enum class GUIImagePosition
	{
		Left, Right
	};

	struct BS_EXPORT GUIElementStyle
	{
		struct BS_EXPORT GUIElementStateStyle
		{
			HSpriteTexture texture;
			Color textColor;
		};

		GUIElementStyle()
			:fontSize(8), width(0), height(0),
			fixedWidth(false), fixedHeight(false), minWidth(0), maxWidth(0),
			minHeight(0), maxHeight(0), textHorzAlign(THA_Left), textVertAlign(TVA_Top), 
			wordWrap(false), imagePosition(GUIImagePosition::Left)
		{

		}

		HFont font;
		UINT32 fontSize;
		TextHorzAlign textHorzAlign;
		TextVertAlign textVertAlign;
		GUIImagePosition imagePosition;
		bool wordWrap;

		GUIElementStateStyle normal;
		GUIElementStateStyle hover;
		GUIElementStateStyle active;
		GUIElementStateStyle focused;

		// For controls that can be turned on-off
		GUIElementStateStyle normalOn;
		GUIElementStateStyle hoverOn;
		GUIElementStateStyle activeOn;
		GUIElementStateStyle focusedOn;

		RectOffset border; // Determines how the element is scaled (using the typical Scale9Grid approach)
		RectOffset margins; // Determines offset from the background graphics to the content. Input uses bounds offset by this value.
		RectOffset contentOffset; // Additional offset to the content, that doesn't effect the bounds. Applied on top of the margins offsets.
		RectOffset padding; // Determines extra distance between this and other elements in a layout

		UINT32 width;
		UINT32 height;
		UINT32 minWidth, maxWidth;
		UINT32 minHeight, maxHeight;
		bool fixedWidth;
		bool fixedHeight;
	};
}