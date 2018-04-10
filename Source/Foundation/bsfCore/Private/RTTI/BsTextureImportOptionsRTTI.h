//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Importer/BsTextureImportOptions.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT TextureImportOptionsRTTI : public RTTIType<TextureImportOptions, ImportOptions, TextureImportOptionsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(mFormat, 0)
			BS_RTTI_MEMBER_PLAIN(mGenerateMips, 1)
			BS_RTTI_MEMBER_PLAIN(mMaxMip, 2)
			BS_RTTI_MEMBER_PLAIN(mCPUCached, 3)
			BS_RTTI_MEMBER_PLAIN(mSRGB, 4)
			BS_RTTI_MEMBER_PLAIN(mCubemap, 5)
			BS_RTTI_MEMBER_PLAIN(mCubemapSourceType, 6)
		BS_END_RTTI_MEMBERS

	public:
		TextureImportOptionsRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "TextureImportOptions";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TextureImportOptions;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<TextureImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}
