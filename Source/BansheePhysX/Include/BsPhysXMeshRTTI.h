//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsRTTIType.h"
#include "BsPhysXMesh.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-PhysX
	 *  @{
	 */

	class BS_PHYSX_EXPORT FPhysXMeshRTTI : public RTTIType<FPhysXMesh, FPhysicsMesh, FPhysXMeshRTTI>
	{
	private:
		ManagedDataBlock getCookedData(FPhysXMesh* obj)
		{
			ManagedDataBlock dataBlock((UINT8*)obj->mCookedData, obj->mCookedDataSize);
			return dataBlock;
		}

		void setCookedData(FPhysXMesh* obj, ManagedDataBlock val)
		{
			// Nothing to do here, the pointer we provided in allocateCookedData() already belongs to PhysXMesh
			// so the data is already written
		}

		static UINT8* allocateCookedData(FPhysXMesh* obj, UINT32 numBytes)
		{
			obj->mCookedData = (UINT8*)bs_alloc(numBytes);
			obj->mCookedDataSize = numBytes;

			return obj->mCookedData;
		}
	public:
		FPhysXMeshRTTI()
		{
			addDataBlockField("mCookedData", 0, 
				&FPhysXMeshRTTI::getCookedData, &FPhysXMeshRTTI::setCookedData, 0, &FPhysXMeshRTTI::allocateCookedData);
		}

		/** @copydoc IReflectable::onDeserializationEnded */
		void onDeserializationEnded(IReflectable* obj) override
		{
			FPhysXMesh* mesh = static_cast<FPhysXMesh*>(obj);
			mesh->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "FPhysXMesh";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_FPhysXMesh;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<FPhysXMesh>();
		}
	};

	/** @} */
	/** @endcond */
}