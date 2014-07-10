//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsRectOffset.h"
#include "BsTextSprite.h"
#include "BsColor.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Possible positions used for positioning content image within a GUI element.
	 */
	enum class GUIImagePosition
	{
		Left, Right
	};

	/**
	 * @brief	GUI element style that determines GUI element look depending on its active state
	 */
	struct BS_EXPORT GUIElementStyle
	{
		/**
		 * @brief	Specific texture and text color used in a particular GUI element style
		 */
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

		HFont font; /**< Font to use for all text within the GUI element. */
		UINT32 fontSize; /**< Font size to use for all text within the GUI element. */
		TextHorzAlign textHorzAlign; /**< Horizontal alignment of text within the GUI element. */
		TextVertAlign textVertAlign; /**< Vertical alignment of text within the GUI element. */
		GUIImagePosition imagePosition; /**< Position of content image relative to text. */
		bool wordWrap; /**< Should the text word wrap if it doesn't fit. */

		GUIElementStateStyle normal; /**< Style used when element is in normal state and off. */
		GUIElementStateStyle hover; /**< Style used when element is in hover state and off. */
		GUIElementStateStyle active; /**< Style used when element is in active state and off. */
		GUIElementStateStyle focused; /**< Style used when element is in focused state and off. */

		// For controls that can be turned on-off
		GUIElementStateStyle normalOn; /**< Style used when element is in normal state and on. */
		GUIElementStateStyle hoverOn; /**< Style used when element is in hover state and on. */
		GUIElementStateStyle activeOn; /**< Style used when element is in active state and on. */
		GUIElementStateStyle focusedOn; /**< Style used when element is in focused state and on. */

		RectOffset border; /**< Determines how the element is scaled (using the typical Scale9Grid approach). */
		RectOffset margins; /**< Determines offset from the background graphics to the content. Input uses bounds offset by this value. */
		RectOffset contentOffset; /**< Additional offset to the content, that doesn't effect the bounds. Applied on top of the margins offsets. */
		RectOffset padding; /**< Determines extra distance between this and other elements in a layout. */

		UINT32 width; /** Wanted width of the GUI element in pixels. Only used if fixedWidth is enabled. */
		UINT32 height; /** Wanted height of the GUI element in pixels. Only used if fixedHeight is enabled. */
		UINT32 minWidth, maxWidth; /**< Minimum and maximum width allowed for this object. Used by the layout only when exact width is not specified. */
		UINT32 minHeight, maxHeight; /**< Minimum and maximum height allowed for this object. Used by the layout only when exact height is not specified. */
		bool fixedWidth; /**< If width is fixed, layout will not attempt to resize the element depending on available size. */
		bool fixedHeight; /**< If height is fixed, layout will not attempt to resize the element depending on available size. */
	};
}