//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCMeshCollider.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT CMeshColliderRTTI : public RTTIType<CMeshCollider, CCollider, CMeshColliderRTTI>
	{
	private:
		BS_REFL_MEMBER(mMesh)

	public:
		CMeshColliderRTTI()
		{
			BS_ADD_REFL_FIELD(mMesh, 0);
		}

		const String& getRTTIName() override
		{
			static String name = "CMeshCollider";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CMeshCollider;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CMeshCollider>();
		}
	};

	/** @} */
	/** @endcond */
}