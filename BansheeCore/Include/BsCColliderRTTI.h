//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCCollider.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CColliderRTTI : public RTTIType<CCollider, Component, CColliderRTTI>
	{
		BS_PLAIN_MEMBER(mLayer)
		BS_PLAIN_MEMBER(mRestOffset)
		BS_PLAIN_MEMBER(mContactOffset)
		BS_REFL_MEMBER(mMaterial)
		BS_PLAIN_MEMBER(mMass)
		BS_PLAIN_MEMBER(mIsTrigger)
		BS_PLAIN_MEMBER(mLocalPosition)
		BS_PLAIN_MEMBER(mLocalRotation)
		BS_PLAIN_MEMBER(mCollisionReportMode)

	public:
		CColliderRTTI()
		{
			BS_ADD_PLAIN_FIELD(mLayer, 0);
			BS_ADD_PLAIN_FIELD(mRestOffset, 1);
			BS_ADD_PLAIN_FIELD(mContactOffset, 2);
			BS_ADD_REFL_FIELD(mMaterial, 3);
			BS_ADD_PLAIN_FIELD(mMass, 4);
			BS_ADD_PLAIN_FIELD(mIsTrigger, 5);
			BS_ADD_PLAIN_FIELD(mLocalPosition, 6);
			BS_ADD_PLAIN_FIELD(mLocalRotation, 7);
			BS_ADD_PLAIN_FIELD(mCollisionReportMode, 8);
		}

		const String& getRTTIName() override
		{
			static String name = "CCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CCollider;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};

	/** @} */
	/** @endcond */
}