//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCAudioSource.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CAudioSourceRTTI : public RTTIType<CAudioSource, Component, CAudioSourceRTTI>
	{
		BS_BEGIN_RTTI_MEMBERS
			//BS_RTTI_MEMBER_REFLPTR(mInternal, 0)
			BS_RTTI_MEMBER_PLAIN(mPlayOnStart, 1)
			BS_RTTI_MEMBER_REFL(mAudioClip, 2)
			BS_RTTI_MEMBER_PLAIN(mVolume, 3)
			BS_RTTI_MEMBER_PLAIN(mPitch, 4)
			BS_RTTI_MEMBER_PLAIN(mLoop, 5)
			BS_RTTI_MEMBER_PLAIN(mPriority, 6)
			BS_RTTI_MEMBER_PLAIN(mMinDistance, 7)
			BS_RTTI_MEMBER_PLAIN(mAttenuation, 8)
		BS_END_RTTI_MEMBERS
	public:
		const String& getRTTIName() override
		{
			static String name = "CAudioSource";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CAudioSource;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<CAudioSource>();
		}
	};

	/** @} */
	/** @endcond */
}
