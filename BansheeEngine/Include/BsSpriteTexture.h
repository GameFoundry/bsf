#pragma once

#include "BsPrerequisites.h"
#include "CmResource.h"
#include "CmVector2.h"

namespace BansheeEngine
{
	/**
	 * @brief	Texture interface that attempts to hide the underlying texture. This primarily allows us
	 * 			to create a sprite texture atlas, without requiring any of the sprite classes to directly
	 * 			know about it.
	 */
	class BS_EXPORT SpriteTexture : public Resource
	{
	public:
		const HTexture& getTexture() const;
		Vector2 transformUV(const Vector2& uv) const;

		static const HSpriteTexture& dummy();
		static HSpriteTexture create(const HTexture& texture);
		static HSpriteTexture create(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture);

		static bool checkIsLoaded(const HSpriteTexture& tex);
	private:
		friend class SpriteTextureRTTI;

		SpriteTexture(const Vector2& uvOffset, const Vector2& uvScale, const HTexture& texture);

		HTexture mAtlasTexture;
		Vector2 mUVOffset;
		Vector2 mUVScale;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

		static SpriteTexturePtr createEmpty();
	public:
		friend class SpriteTextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}