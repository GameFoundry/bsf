#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of GUI element states.
	 */
	enum class GUIElementState
	{
		Normal = 0x01, /**< Normal state when button is not being iteracted with. */
		Hover = 0x02, /**< State when pointer is hovering over the button. */
		Active = 0x04, /**< State when button is being clicked. */
		Focused = 0x08, /**< State when button has been selected. */
		NormalOn = 0x11, /**< Normal state when button is not being iteracted with and is in "on" state. */
		HoverOn = 0x12, /**< State when pointer is hovering over the button and is in "on" state. */
		ActiveOn = 0x14, /**< State when button is being clicked and is in "on" state. */
		FocusedOn = 0x18 /**< State when button has been selected and is in "on" state. */
	};

	/**
	 * @brief	Contains separate GUI content images for every possible GUI element state.
	 */
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
	 * @brief	Holds data used for displaying content in a GUIElement. 
	 *			Content can consist of a string, image, a tooltip or none of those.
	 */
	class BS_EXPORT GUIContent
	{
	public:
		/**
		 * @brief	Constructs an empty content.
		 */
		GUIContent();

		/**
		 * @brief	Constructs content with just a string.
		 */
		GUIContent(const HString& text);

		/**
		 * @brief	Constructs content with a string and a tooltip.
		 */
		GUIContent(const HString& text, const HString& tooltip);

		/**
		 * @brief	Constructs content with just an image.
		 */
		GUIContent(const GUIContentImages& image);

		/**
		 * @brief	Constructs content with an image and a tooltip.
		 */
		GUIContent(const HSpriteTexture& image, const HString& tooltip);

		/**
		 * @brief	Constructs content with a string and an image. 
		 */
		GUIContent(const HString& text, const GUIContentImages& image);

		/**
		 * @brief	Constructs content with a string, an image and a tooltip. 
		 */
		GUIContent(const HString& text, const GUIContentImages& image, const HString& tooltip);

		/**
		 * @brief	Returns string content (if any).
		 */
		const HString& getText() const { return mText; }

		/**
		 * @brief	Returns image content (if any).
		 */
		const HSpriteTexture& getImage(GUIElementState state = GUIElementState::Normal) const;

		/**
		 * @brief	Returns tooltip content (if any).
		 */
		const HString& getTooltip() const { return mTooltipText; }

		/**
		 * @brief	Determines the spacing between text and image content in pixels.
		 */
		static const UINT32 IMAGE_TEXT_SPACING;
	private:
		HString mText;
		GUIContentImages mImages;
		HString mTooltipText;
	};
}