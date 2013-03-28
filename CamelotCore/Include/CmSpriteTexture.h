#pragma once

#include "CmPrerequisites.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	class CM_EXPORT SpriteTexture
	{
	public:
		const TextureHandle& getBaseTexture() const { return mBaseTexture; }

	private:
		TextureHandle mBaseTexture;
		Vector2 mUVOffset;
		Vector2 mUVScale;
	};
}