//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCSphereCollider.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CSphereColliderRTTI : public RTTIType<CSphereCollider, CCollider, CSphereColliderRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mRadius)

	public:
		CSphereColliderRTTI()
		{
			BS_ADD_PLAIN_FIELD(mRadius, 0);
		}

		const String& getRTTIName() override
		{
			static String name = "CSphereCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CSphereCollider;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CSphereCollider>();
		}
	};

	/** @} */
	/** @endcond */
}