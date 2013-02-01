#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMesh.h"
#include "CmMeshManager.h"

namespace CamelotEngine
{
	class MeshRTTI : public RTTIType<Mesh, Resource, MeshRTTI>
	{
		MeshDataPtr getMeshData(Mesh* obj) { return obj->getMeshData(); }
		void setMeshData(Mesh* obj, MeshDataPtr meshData) { obj->setMeshData(meshData); }

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