//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "2D/BsSpriteTexture.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT SpriteTextureRTTI : public RTTIType<SpriteTexture, Resource, SpriteTextureRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(mAtlasTexture, 0)
			BS_RTTI_MEMBER_PLAIN(mUVOffset, 1)
			BS_RTTI_MEMBER_PLAIN(mUVScale, 2)
		BS_END_RTTI_MEMBERS

	public:
		SpriteTextureRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "SpriteTexture";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_SpriteTexture;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SpriteTexture::createEmpty();
		}
	};

	/** @} */
	/** @endcond */
}
