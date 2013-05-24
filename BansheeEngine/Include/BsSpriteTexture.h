#pragma once

#include "BsPrerequisites.h"
#include "CmVector2.h"

namespace BansheeEngine
{
	/**
	 * @brief	Texture interface that attempts to hide the underlying texture. This primarily allows us
	 * 			to create a sprite texture atlas, without requiring any of the sprite classes to directly
	 * 			know about it.
	 */
	class BS_EXPORT SpriteTexture
	{
	public:
		SpriteTexture(const CM::HTexture& baseTexture);

		const CM::HTexture& getTexture() const;
		CM::Vector2 transformUV(const CM::Vector2& uv) const;

		static SpriteTexturePtr dummy();
	private:
		CM::HTexture mBaseTexture;

		bool mUsingAtlas;
		CM::HTexture mAtlasTexture;
		CM::Vector2 mUVOffset;
		CM::Vector2 mUVScale;
	};
}