//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCCharacterController.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CCharacterControllerRTTI : public RTTIType<CCharacterController, Component, CCharacterControllerRTTI>
	{
	private:
		BS_PLAIN_MEMBER_NAMED(mPosition, mDesc.position)
		BS_PLAIN_MEMBER_NAMED(mContactOffset, mDesc.contactOffset)
		BS_PLAIN_MEMBER_NAMED(mStepOffset, mDesc.stepOffset)
		BS_PLAIN_MEMBER_NAMED(mSlopeLimit, mDesc.slopeLimit)
		BS_PLAIN_MEMBER_NAMED(mMinMoveDistance, mDesc.minMoveDistance)
		BS_PLAIN_MEMBER_NAMED(mHeight, mDesc.height)
		BS_PLAIN_MEMBER_NAMED(mRadius, mDesc.radius)
		BS_PLAIN_MEMBER_NAMED(mUp, mDesc.up)
		BS_PLAIN_MEMBER_NAMED(mClimbingMode, mDesc.climbingMode)
		BS_PLAIN_MEMBER_NAMED(mNonWalkableMode, mDesc.nonWalkableMode)
		BS_PLAIN_MEMBER(mLayer)

	public:
		CCharacterControllerRTTI()
		{
			BS_ADD_PLAIN_FIELD(mPosition, 0);
			BS_ADD_PLAIN_FIELD(mContactOffset, 1);
			BS_ADD_PLAIN_FIELD(mStepOffset, 2);
			BS_ADD_PLAIN_FIELD(mSlopeLimit, 3);
			BS_ADD_PLAIN_FIELD(mMinMoveDistance, 4);
			BS_ADD_PLAIN_FIELD(mHeight, 5);
			BS_ADD_PLAIN_FIELD(mRadius, 6);
			BS_ADD_PLAIN_FIELD(mUp, 7);
			BS_ADD_PLAIN_FIELD(mClimbingMode, 8);
			BS_ADD_PLAIN_FIELD(mNonWalkableMode, 9);
			BS_ADD_PLAIN_FIELD(mLayer, 10);
		}

		const String& getRTTIName() override
		{
			static String name = "CCharacterController";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CCharacterController;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CCharacterController>();
		}
	};

	/** @} */
	/** @endcond */
}