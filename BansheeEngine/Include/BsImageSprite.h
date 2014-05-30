#pragma once

#include "BsPrerequisites.h"
#include "BsSprite.h"
#include "BsVector2.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Image sprite description structure used for initializing or updating an image sprite.
	 */
	struct IMAGE_SPRITE_DESC
	{
		IMAGE_SPRITE_DESC()
			:width(0), height(0), anchor(SA_TopLeft), borderLeft(0), borderRight(0), 
			borderTop(0), borderBottom(0), uvScale(1.0f, 1.0f), uvOffset(0.0f, 0.0f)
		{ }

		UINT32 width; /**< Width of the image in pixels. */
		UINT32 height; /**< Height of the image in pixels. */
		SpriteAnchor anchor; /**< Determines where in the provided bounds will the sprite be placed. */
		Vector2 uvScale; /**< Scale applied to UV width/height used for rendering the sprite. */
		Vector2 uvOffset; /**< Offset applied to UV coordinates when rendering the sprite. */

		SpriteTexturePtr texture; /**< Texture to overlay on the sprite. */
		Color color; /**< Color tint to apply to the sprite. */

		/** Borders (in texels) that allow you to control how is the texture scaled. 
		 * If borders are 0 the texture will be scaled uniformly. If they are not null
		 * only the area inside the borders will be scaled and the outside are will remain
		 * the original size as in the texture. This allows you to implement "Scale9Grid"
		 * functionality.
		 */
		UINT32 borderLeft, borderRight, borderTop, borderBottom; 
	};

	/**
	 * @brief	A sprite consisting of a single image represented by a sprite texture.
	 */
	class BS_EXPORT ImageSprite : public Sprite
	{
	public:
		ImageSprite();

		/**
		 * @brief	Recreates internal sprite data according the specified description structure.
		 */
		void update(const IMAGE_SPRITE_DESC& desc);
	};
}