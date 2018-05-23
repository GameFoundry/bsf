//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once
#include "BsPrerequisites.h"

namespace bs
{
	/** @addtogroup GUI
	 *  @{
	 */

	/**	Type of GUI element states. */
	enum class GUIElementState
	{
		Normal = 0x01, /**< Normal state when element is not being iteracted with. */
		Hover = 0x02, /**< State when pointer is hovering over the element. */
		Active = 0x04, /**< State when element is being clicked. */
		Focused = 0x08, /**< State when the element has input focus and pointer is not hovering over the element. */
		FocusedHover = 0x10, /**< State when the element has input focus and pointer is hovering over the element. */
		NormalOn = 0x101, /**< Same as Normal, if the element is also in the "on" state. */
		HoverOn = 0x102, /**< Same as Hover, if the element is also in the "on" state. */
		ActiveOn = 0x104, /**< Same as Active, if the element is also in the "on" state. */
		FocusedOn = 0x108, /**< Same as Focused, if the element is also in the "on" state. */
		FocusedHoverOn = 0x110, /**< Same as FocusedHover, if the element is also in the "on" state. */

		// Helpers
		TypeMask = 0xFF, /**< Mask for determining the state type (ignoring the on state). */
		OnFlag = 0x100 /**< Flag that differentiates between on and off states. */
	};

	/**	Contains separate GUI content images for every possible GUI element state. */
	struct BS_EXPORT GUIContentImages
	{
		GUIContentImages() {}
		GUIContentImages(const HSpriteTexture& image);

		HSpriteTexture normal;
		HSpriteTexture hover;
		HSpriteTexture active;
		HSpriteTexture focused;
		HSpriteTexture normalOn;
		HSpriteTexture hoverOn;
		HSpriteTexture activeOn;
		HSpriteTexture focusedOn;
	};

	/**
	 * Holds data used for displaying content in a GUIElement. Content can consist of a string, image, a tooltip or none 
	 * of those.
	 */
	class BS_EXPORT GUIContent
	{
	public:
		/**	Constructs an empty content. */
		GUIContent() = default;

		/**	Constructs content with just a string. */
		GUIContent(const HString& text);

		/**	Constructs content with a string and a tooltip. */
		GUIContent(const HString& text, const HString& tooltip);

		/**	Constructs content with just an image. */
		GUIContent(const GUIContentImages& image);

		/**	Constructs content with an image and a tooltip. */
		GUIContent(const HSpriteTexture& image, const HString& tooltip);

		/**	Constructs content with a string and an image. */
		GUIContent(const HString& text, const GUIContentImages& image);

		/**	Constructs content with a string, an image and a tooltip. */
		GUIContent(const HString& text, const GUIContentImages& image, const HString& tooltip);

		/**	Returns string content (if any). */
		const HString& getText() const { return mText; }

		/**	Returns image content (if any). */
		const HSpriteTexture& getImage(GUIElementState state = GUIElementState::Normal) const;

		/**	Returns tooltip content (if any). */
		const HString& getTooltip() const { return mTooltipText; }

		/**	Determines the spacing between text and image content in pixels. */
		static const UINT32 IMAGE_TEXT_SPACING;
	private:
		HString mText;
		GUIContentImages mImages;
		HString mTooltipText;
	};

	/** @} */
}