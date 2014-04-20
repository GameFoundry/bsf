#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIContent
	{
	public:
		GUIContent();
		explicit GUIContent(const HString& text);
		GUIContent(const HString& text, const HString& tooltip);

		explicit GUIContent(const HSpriteTexture& image);
		GUIContent(const HSpriteTexture& image, const HString& tooltip);

		GUIContent(const HString& text, const HSpriteTexture& image);
		GUIContent(const HString& text, const HSpriteTexture& image, const HString& tooltip);

		const HString& getText() const { return mText; }
		const HSpriteTexture& getImage() const { return mImage; }
		const HString& getTooltip() const { return mTooltipText; }

	private:
		HString mText;
		HSpriteTexture mImage;
		HString mTooltipText;
	};
}