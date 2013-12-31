#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	class BS_EXPORT SpriteTextureRTTI : public CM::RTTIType<SpriteTexture, CM::Resource, SpriteTextureRTTI>
	{
	private:
		CM::HTexture& getAtlasTexture(SpriteTexture* obj) { return obj->mAtlasTexture; }
		void setAtlasTexture(SpriteTexture* obj, CM::HTexture& val) { obj->mAtlasTexture = val; } 

		CM::Vector2& getUVOffset(SpriteTexture* obj) { return obj->mUVOffset; }
		void setUVOffset(SpriteTexture* obj, CM::Vector2& val) { obj->mUVOffset = val; } 

		CM::Vector2& getUVScale(SpriteTexture* obj) { return obj->mUVScale; }
		void setUVScale(SpriteTexture* obj, CM::Vector2& val) { obj->mUVScale = val; } 

	public:
		SpriteTextureRTTI()
		{
			addReflectableField("mAtlasTexture", 0, &SpriteTextureRTTI::getAtlasTexture, &SpriteTextureRTTI::setAtlasTexture);
			addPlainField("mUVOffset", 1, &SpriteTextureRTTI::getUVOffset, &SpriteTextureRTTI::setUVOffset);
			addPlainField("mUVScale", 2, &SpriteTextureRTTI::getUVScale, &SpriteTextureRTTI::setUVScale);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "SpriteTexture";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_SpriteTexture;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return SpriteTexture::createEmpty();
		}
	};
}