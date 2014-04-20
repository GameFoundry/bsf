#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	class BS_EXPORT SpriteTextureRTTI : public RTTIType<SpriteTexture, Resource, SpriteTextureRTTI>
	{
	private:
		HTexture& getAtlasTexture(SpriteTexture* obj) { return obj->mAtlasTexture; }
		void setAtlasTexture(SpriteTexture* obj, HTexture& val) { obj->mAtlasTexture = val; } 

		Vector2& getUVOffset(SpriteTexture* obj) { return obj->mUVOffset; }
		void setUVOffset(SpriteTexture* obj, Vector2& val) { obj->mUVOffset = val; } 

		Vector2& getUVScale(SpriteTexture* obj) { return obj->mUVScale; }
		void setUVScale(SpriteTexture* obj, Vector2& val) { obj->mUVScale = val; } 

	public:
		SpriteTextureRTTI()
		{
			addReflectableField("mAtlasTexture", 0, &SpriteTextureRTTI::getAtlasTexture, &SpriteTextureRTTI::setAtlasTexture);
			addPlainField("mUVOffset", 1, &SpriteTextureRTTI::getUVOffset, &SpriteTextureRTTI::setUVOffset);
			addPlainField("mUVScale", 2, &SpriteTextureRTTI::getUVScale, &SpriteTextureRTTI::setUVScale);
		}

		virtual const String& getRTTIName()
		{
			static String name = "SpriteTexture";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SpriteTexture;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return SpriteTexture::createEmpty();
		}
	};
}