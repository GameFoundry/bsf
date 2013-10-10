#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIContent
	{
	public:
		explicit GUIContent(const CM::HString& text);
		GUIContent(const CM::HString& text, const CM::HString& tooltip);

		explicit GUIContent(const SpriteTexturePtr& image);
		GUIContent(const SpriteTexturePtr& image, const CM::HString& tooltip);

		GUIContent(const CM::HString& text, const SpriteTexturePtr& image);
		GUIContent(const CM::HString& text, const SpriteTexturePtr& image, const CM::HString& tooltip);

		const CM::HString& getText() const { return mText; }
		const SpriteTexturePtr& getImage() const { return mImage; }
		const CM::HString& getTooltip() const { return mTooltipText; }

	private:
		CM::HString mText;
		SpriteTexturePtr mImage;
		CM::HString mTooltipText;
	};
}