#pragma once
#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIContent
	{
	public:
		GUIContent(const CM::WString& text);
		GUIContent(const CM::WString& text, const CM::WString& tooltip);

		GUIContent(const SpriteTexturePtr& image);
		GUIContent(const SpriteTexturePtr& image, const CM::WString& tooltip);

		GUIContent(const CM::WString& text, const SpriteTexturePtr& image);
		GUIContent(const CM::WString& text, const SpriteTexturePtr& image, const CM::WString& tooltip);

		const CM::WString& getText() const { return mText; }
		const SpriteTexturePtr& getImage() const { return mImage; }
		const CM::WString& getTooltip() const { return mTooltipText; }

	private:
		CM::WString mText;
		SpriteTexturePtr mImage;
		CM::WString mTooltipText;
	};
}