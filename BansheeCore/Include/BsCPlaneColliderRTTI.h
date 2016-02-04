//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCPlaneCollider.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CPlaneColliderRTTI : public RTTIType<CPlaneCollider, CCollider, CPlaneColliderRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mNormal)
		BS_PLAIN_MEMBER(mDistance)
	public:
		CPlaneColliderRTTI()
		{
			BS_ADD_PLAIN_FIELD(mNormal, 0);
			BS_ADD_PLAIN_FIELD(mDistance, 1);
		}

		const String& getRTTIName() override
		{
			static String name = "CPlaneCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CPlaneCollider;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CPlaneCollider>();
		}
	};

	/** @} */
	/** @endcond */
}