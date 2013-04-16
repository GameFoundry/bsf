#pragma once

#include "CmPrerequisites.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	/**
	 * @brief	Texture interface that attempts to hide the underlying texture. This primarily allows us
	 * 			to create a sprite texture atlas, without requiring any of the sprite classes to directly
	 * 			know about it.
	 */
	class CM_EXPORT SpriteTexture
	{
	public:
		SpriteTexture(const HTexture& baseTexture);

		const HTexture& getTexture() const;
		Vector2 transformUV(const Vector2& uv) const;

	private:
		HTexture mBaseTexture;

		bool mUsingAtlas;
		HTexture mAtlasTexture;
		Vector2 mUVOffset;
		Vector2 mUVScale;
	};
}