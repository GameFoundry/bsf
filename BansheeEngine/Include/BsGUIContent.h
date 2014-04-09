#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIContent
	{
	public:
		GUIContent();
		explicit GUIContent(const CM::HString& text);
		GUIContent(const CM::HString& text, const CM::HString& tooltip);

		explicit GUIContent(const HSpriteTexture& image);
		GUIContent(const HSpriteTexture& image, const CM::HString& tooltip);

		GUIContent(const CM::HString& text, const HSpriteTexture& image);
		GUIContent(const CM::HString& text, const HSpriteTexture& image, const CM::HString& tooltip);

		const CM::HString& getText() const { return mText; }
		const HSpriteTexture& getImage() const { return mImage; }
		const CM::HString& getTooltip() const { return mTooltipText; }

	private:
		CM::HString mText;
		HSpriteTexture mImage;
		CM::HString mTooltipText;
	};
}