//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "Reflection/BsIReflectable.h"
#include "Utility/BsRectOffset.h"
#include "2D/BsTextSprite.h"
#include "Image/BsColor.h"
#include "Math/BsVector2I.h"

namespace bs
{
	/** @addtogroup GUI
	 *  @{
	 */

	/**	Possible positions used for positioning content image within a GUI element. */
	enum class BS_SCRIPT_EXPORT(m:GUI) GUIImagePosition
	{
		Left, Right
	};

	/**	Specific texture and text color used in a particular GUI element style. */
	struct BS_EXPORT BS_SCRIPT_EXPORT(m:GUI,pl:true) GUIElementStateStyle
	{
		BS_NORREF HSpriteTexture texture;
		Color textColor;
	};

	/**
	 * GUI element style that determines the look of a GUI element, as well as the element's default layout options.
	 * Different looks can be provided for different element states.
	 */
	struct BS_EXPORT BS_SCRIPT_EXPORT(m:GUI) GUIElementStyle : public IReflectable
	{
		BS_SCRIPT_EXPORT()
		GUIElementStyle() = default;

		BS_SCRIPT_EXPORT()
		BS_NORREF HFont font; /**< Font to use for all text within the GUI element. */

		BS_SCRIPT_EXPORT()
		UINT32 fontSize = 8; /**< Font size to use for all text within the GUI element. */

		BS_SCRIPT_EXPORT()
		TextHorzAlign textHorzAlign = THA_Left; /**< Horizontal alignment of text within the GUI element. */

		BS_SCRIPT_EXPORT()
		TextVertAlign textVertAlign = TVA_Top; /**< Vertical alignment of text within the GUI element. */

		BS_SCRIPT_EXPORT()
		GUIImagePosition imagePosition = GUIImagePosition::Left; /**< Position of content image relative to text. */

		BS_SCRIPT_EXPORT()
		bool wordWrap = false; /**< Should the text word wrap if it doesn't fit. */

		/**
		 * Style used when the element doesn't have focus nor is the user interacting with the element. Used when the
		 * element is in the 'off' state.
		 */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle normal;

		/**
		 * Style used when the user is hovering the pointer over the element, while the element doesn't have focus. Used
		 * when the element is in the 'off' state.
		 */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle hover;

		/**
		 * Style used when the user is actively interacting with the element. Used when the element is in the 'off' state.
		 */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle active;

		/**
		 * Style used when the element has focus but the pointer is not hovering over the element. Used when the element is
		 * in the 'off' state.
		 */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle focused;

		/**
		 * Style used when the element has focus and the pointer is hovering over the element. Used when the element is
		 * in the 'off' state.
		 */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle focusedHover;

		/** Same as GUIElementStyle::normal, except it's used when element is in the 'on' state. */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle normalOn;

		/** Same as GUIElementStyle::hover, except it's used when element is in the 'on' state. */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle hoverOn;

		/** Same as GUIElementStyle::active, except it's used when element is in the 'on' state. */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle activeOn;

		/** Same as GUIElementStyle::focused, except it's used when element is in the 'on' state. */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle focusedOn;

		/** Same as GUIElementStyle::focusedHover, except it's used when element is in the 'on' state. */
		BS_SCRIPT_EXPORT()
		GUIElementStateStyle focusedHoverOn;

		BS_SCRIPT_EXPORT()
		RectOffset border; /**< Determines how the element is scaled (using the typical Scale9Grid approach). */

		BS_SCRIPT_EXPORT()
		RectOffset margins; /**< Determines offset from the background graphics to the content. Input uses bounds offset by this value. */

		BS_SCRIPT_EXPORT()
		RectOffset contentOffset; /**< Additional offset to the content, that doesn't effect the bounds. Applied on top of the margins offsets. */

		BS_SCRIPT_EXPORT()
		RectOffset padding; /**< Determines extra distance between this and other elements in a layout. */

		BS_SCRIPT_EXPORT()
		UINT32 width = 0; /**< Wanted width of the GUI element in pixels. Only used if fixedWidth is enabled. */

		BS_SCRIPT_EXPORT()
		UINT32 height = 0; /**< Wanted height of the GUI element in pixels. Only used if fixedHeight is enabled. */

		BS_SCRIPT_EXPORT()
		UINT32 minWidth = 0; /**< Minimum width allowed for the GUI element. Used by the layout only when exact width is not specified. */

		BS_SCRIPT_EXPORT()
		UINT32 maxWidth = 0; /**< Maximum width allowed for the GUI element. Used by the layout only when exact width is not specified. */

		BS_SCRIPT_EXPORT()
		UINT32 minHeight = 0; /**< Minimum height allowed for the GUI element. Used by the layout only when exact height is not specified. */

		BS_SCRIPT_EXPORT()
		UINT32 maxHeight = 0; /**< Maximum height allowed for the GUI element. Used by the layout only when exact height is not specified. */

		BS_SCRIPT_EXPORT()
		bool fixedWidth = false; /**< Determines should the layout resize the element depending on available size. If true no resizing will be done. */

		BS_SCRIPT_EXPORT()
		bool fixedHeight = false; /**< Determines should the layout resize the element depending on available size. If true no resizing will be done. */

		/**
		 * Registers a new sub-style that is used by complex GUI elements that contain one or multiple sub-elements.
		 * @param[in]	guiType		Name of the sub-element this style is to be used for.  This depends on GUI element the
		 *							style is applied to.
		 * @param[in]	styleName	Name of the style in GUI skin to use for the sub-element.
		 */
		BS_SCRIPT_EXPORT()
		void addSubStyle(const String& guiType, const String& styleName)
		{
			subStyles[guiType] = styleName;
		}

		Map<String, String> subStyles; /**< Sub-styles used by certain more complex elements. */

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GUIElementStyleRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}
