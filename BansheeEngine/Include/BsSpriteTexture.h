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
	class BS_EXPORT SpriteTexture : public CM::Resource
	{
	public:
		const CM::HTexture& getTexture() const;
		CM::Vector2 transformUV(const CM::Vector2& uv) const;

		static const HSpriteTexture& dummy();
		static HSpriteTexture create(const CM::HTexture& texture);
		static HSpriteTexture create(const CM::Vector2& uvOffset, const CM::Vector2& uvScale, const CM::HTexture& texture);
	private:
		friend class SpriteTextureRTTI;

		SpriteTexture(const CM::Vector2& uvOffset, const CM::Vector2& uvScale, const CM::HTexture& texture);

		CM::HTexture mAtlasTexture;
		CM::Vector2 mUVOffset;
		CM::Vector2 mUVScale;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

		static SpriteTexturePtr createEmpty();
	public:
		friend class SpriteTextureRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;	
	};
}