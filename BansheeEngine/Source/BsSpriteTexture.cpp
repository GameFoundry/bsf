#include "BsSpriteTexture.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
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

	SpriteTexturePtr SpriteTexture::dummy()
	{
		static bool initialized = false;
		static SpriteTexturePtr dummyTex = nullptr;

		if(!initialized)
		{
			dummyTex = cm_shared_ptr<SpriteTexture, PoolAlloc>(Texture::dummy());
		}

		return dummyTex;
	}
}