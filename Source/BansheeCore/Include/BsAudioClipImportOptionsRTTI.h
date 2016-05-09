//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsAudioClipImportOptions.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT AudioClipImportOptionsRTTI : public RTTIType<AudioClipImportOptions, ImportOptions, AudioClipImportOptionsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mFormat);
		BS_PLAIN_MEMBER(mReadMode);

	public:
		AudioClipImportOptionsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mFormat, 0);
			BS_ADD_PLAIN_FIELD(mReadMode, 1);
		}

		/** @copydoc RTTIType::getRTTIName */
		const String& getRTTIName() override
		{
			static String name = "AudioClipImportOptions";
			return name;
		}

		/** @copydoc RTTIType::getRTTIId */
		UINT32 getRTTIId() override
		{
			return TID_AudioClipImportOptions;
		}

		/** @copydoc RTTIType::newRTTIObject */
		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<AudioClipImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}