#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"
#include "CmVector2.h"

namespace BansheeEngine
{
	struct IMAGE_SPRITE_DESC
	{
		IMAGE_SPRITE_DESC()
			:width(0), height(0), anchor(SA_TopLeft), borderLeft(0), borderRight(0), 
			borderTop(0), borderBottom(0), uvScale(1.0f, 1.0f), uvOffset(0.0f, 0.0f)
		{ }

		UINT32 width;
		UINT32 height;
		SpriteAnchor anchor;
		Vector2 uvScale;
		Vector2 uvOffset;

		SpriteTexturePtr texture;
		Color color;
		UINT32 borderLeft, borderRight, borderTop, borderBottom;
	};

	class BS_EXPORT ImageSprite : public Sprite
	{
	public:
		ImageSprite();

		void update(const IMAGE_SPRITE_DESC& desc);
	};
}