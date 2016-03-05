//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsMeshImportOptions.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT MeshImportOptionsRTTI : public RTTIType <MeshImportOptions, ImportOptions, MeshImportOptionsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mCPUReadable)
		BS_PLAIN_MEMBER(mImportNormals)
		BS_PLAIN_MEMBER(mImportTangents)
		BS_PLAIN_MEMBER(mImportBlendShapes)
		BS_PLAIN_MEMBER(mImportSkin)
		BS_PLAIN_MEMBER(mImportAnimation)
		BS_PLAIN_MEMBER(mImportScale)
		BS_PLAIN_MEMBER(mCollisionMeshType)

	public:
		MeshImportOptionsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mCPUReadable, 0);
			BS_ADD_PLAIN_FIELD(mImportNormals, 1);
			BS_ADD_PLAIN_FIELD(mImportTangents, 2);
			BS_ADD_PLAIN_FIELD(mImportBlendShapes, 3);
			BS_ADD_PLAIN_FIELD(mImportSkin, 4);
			BS_ADD_PLAIN_FIELD(mImportAnimation, 5);
			BS_ADD_PLAIN_FIELD(mImportScale, 6);
			BS_ADD_PLAIN_FIELD(mCollisionMeshType, 7);
		}

		const String& getRTTIName() override
		{
			static String name = "MeshImportOptions";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_MeshImportOptions;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<MeshImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}