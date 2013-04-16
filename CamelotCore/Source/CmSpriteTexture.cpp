#include "CmSpriteTexture.h"

namespace CamelotEngine
{
	SpriteTexture::SpriteTexture(const HTexture& baseTexture)
		:mBaseTexture(baseTexture), mUsingAtlas(false)
	{

	}

	const HTexture& SpriteTexture::getTexture() const 
	{ 
		return mUsingAtlas ? mAtlasTexture : mBaseTexture; 
	}

	Vector2 SpriteTexture::transformUV(const Vector2& uv) const
	{
		if(mUsingAtlas)
			return mUVOffset + uv * mUVScale;
		else
			return uv;
	}
}