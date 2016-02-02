//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

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

		const String& getRTTIName() override
		{
			static String name = "SpriteTexture";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_SpriteTexture;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return SpriteTexture::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}