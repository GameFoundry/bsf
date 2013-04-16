#pragma once

#include "CmPrerequisites.h"
#include "CmSprite.h"

namespace CamelotEngine
{
	struct IMAGE_SPRITE_DESC
	{
		IMAGE_SPRITE_DESC()
			:width(0), height(0), anchor(SA_TopLeft), borderLeft(0), borderRight(0), 
			borderTop(0), borderBottom(0)
		{ }

		Point offset;
		UINT32 width;
		UINT32 height;
		Rect clipRect;
		SpriteAnchor anchor;

		SpriteTexturePtr texture;
		UINT32 borderLeft, borderRight, borderTop, borderBottom;
	};

	class CM_EXPORT ImageSprite : public Sprite
	{
	public:
		ImageSprite();

		void update(const IMAGE_SPRITE_DESC& desc);
	};
}