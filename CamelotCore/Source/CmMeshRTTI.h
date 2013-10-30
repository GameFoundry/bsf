#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmApplication.h"
#include "CmMesh.h"
#include "CmMeshManager.h"

namespace CamelotFramework
{
	class MeshRTTI : public RTTIType<Mesh, GpuResource, MeshRTTI>
	{
		MeshDataPtr getMeshData(Mesh* obj) 
		{ 
			MeshDataPtr meshData = obj->allocateSubresourceBuffer(0);

			GpuResourcePtr sharedMeshPtr = std::static_pointer_cast<GpuResource>(obj->getThisPtr());

			gMainSyncedCA().readSubresource(sharedMeshPtr, 0, meshData);
			gMainSyncedCA().submitToCoreThread(true); // We need the data right away, so execute the context and wait until we get it

			return meshData;
		}

		void setMeshData(Mesh* obj, MeshDataPtr meshData) 
		{ 
			GpuResourcePtr sharedMeshPtr = std::static_pointer_cast<GpuResource>(obj->getThisPtr());

			gMainSyncedCA().writeSubresource(sharedMeshPtr, 0, meshData);
		}

	public:
		MeshRTTI()
		{
			addReflectablePtrField("mMeshData", 0, &MeshRTTI::getMeshData, &MeshRTTI::setMeshData);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return MeshManager::instance().create();
		}

		virtual const String& getRTTIName() 
		{
			static String name = "Mesh";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_Mesh;
		}
	};
}