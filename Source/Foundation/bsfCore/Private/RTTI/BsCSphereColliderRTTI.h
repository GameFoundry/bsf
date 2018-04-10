//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCSphereCollider.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CSphereColliderRTTI : public RTTIType<CSphereCollider, CCollider, CSphereColliderRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(mRadius, 0)
		BS_END_RTTI_MEMBERS
	public:
		CSphereColliderRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CSphereCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CSphereCollider;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CSphereCollider>();
		}
	};

	/** @} */
	/** @endcond */
}
