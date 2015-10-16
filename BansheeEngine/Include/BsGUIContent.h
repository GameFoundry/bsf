#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
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
		GUIContent(const HSpriteTexture& image);

		/**
		 * @brief	Constructs content with an image and a tooltip.
		 */
		GUIContent(const HSpriteTexture& image, const HString& tooltip);

		/**
		 * @brief	Constructs content with a string and an image.
		 */
		GUIContent(const HString& text, const HSpriteTexture& image);

		/**
		 * @brief	Constructs content with a string, an image and a tooltip.
		 */
		GUIContent(const HString& text, const HSpriteTexture& image, const HString& tooltip);

		/**
		 * @brief	Returns string content (if any).
		 */
		const HString& getText() const { return mText; }

		/**
		 * @brief	Returns image content (if any).
		 */
		const HSpriteTexture& getImage() const { return mImage; }

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
		HSpriteTexture mImage;
		HString mTooltipText;
	};
}