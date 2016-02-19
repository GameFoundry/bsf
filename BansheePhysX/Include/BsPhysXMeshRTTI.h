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

	class BS_PHYSX_EXPORT PhysXMeshRTTI : public RTTIType<PhysXMesh, PhysicsMesh, PhysXMeshRTTI>
	{
	private:
		ManagedDataBlock getCookedData(PhysXMesh* obj)
		{
			ManagedDataBlock dataBlock((UINT8*)obj->mCookedData, obj->mCookedDataSize);
			return dataBlock;
		}

		void setCookedData(PhysXMesh* obj, ManagedDataBlock val)
		{
			// Nothing to do here, the pointer we provided in allocateCookedData() already belongs to PhysXMesh
			// so the data is already written
		}

		static UINT8* allocateCookedData(PhysXMesh* obj, UINT32 numBytes)
		{
			obj->mCookedData = (UINT8*)bs_alloc(numBytes);
			obj->mCookedDataSize = numBytes;

			return obj->mCookedData;
		}
	public:
		PhysXMeshRTTI()
		{
			addDataBlockField("mCookedData", 0, 
				&PhysXMeshRTTI::getCookedData, &PhysXMeshRTTI::setCookedData, 0, &PhysXMeshRTTI::allocateCookedData);
		}

		void onDeserializationEnded(IReflectable* obj) override
		{
			PhysXMesh* mesh = static_cast<PhysXMesh*>(obj);
			mesh->initialize();
		}

		const String& getRTTIName() override
		{
			static String name = "PhysXMesh";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_PhysXMesh;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return PhysicsMesh::_createPtr(nullptr, PhysicsMeshType::Convex);
		}
	};

	/** @} */
	/** @endcond */
}