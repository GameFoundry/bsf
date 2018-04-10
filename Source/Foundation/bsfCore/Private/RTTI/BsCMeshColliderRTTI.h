//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCMeshCollider.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CMeshColliderRTTI : public RTTIType<CMeshCollider, CCollider, CMeshColliderRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(mMesh, 0)
		BS_END_RTTI_MEMBERS
	public:
		CMeshColliderRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CMeshCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CMeshCollider;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CMeshCollider>();
		}
	};

	/** @} */
	/** @endcond */
}
