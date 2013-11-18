#include "CmMeshManager.h"
#include "CmCoreThreadAccessor.h"
#include "CmApplication.h"
#include "CmVector3.h"
#include "CmMesh.h"
#include "CmVertexDataDesc.h"

namespace CamelotFramework
{
	MeshManager::MeshManager()
	{

	}

	MeshManager::~MeshManager()
	{

	}

	MeshPtr MeshManager::create()
	{
		MeshPtr mesh = cm_core_ptr<Mesh, PoolAlloc>(new (cm_alloc<Mesh, PoolAlloc>()) Mesh());
		mesh->setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::createEmpty()
	{
		MeshPtr mesh = cm_core_ptr<Mesh, PoolAlloc>(new (cm_alloc<Mesh, PoolAlloc>()) Mesh());
		mesh->setThisPtr(mesh);

		return mesh;
	}

	void MeshManager::onStartUp()
	{
		VertexDataDescPtr vertexDesc = cm_shared_ptr<VertexDataDesc>();
		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		mDummyMeshData = cm_shared_ptr<MeshData>(1, 3, vertexDesc);

		auto vecIter = mDummyMeshData->getVec3DataIter(VES_POSITION);
		vecIter.setValue(Vector3(0, 0, 0));

		auto indices = mDummyMeshData->getIndices32(0);
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;

		SyncedCoreAccessor& coreAccessor = gMainSyncedCA();

		mDummyMesh = Mesh::create();
		coreAccessor.writeSubresource(mDummyMesh.getInternalPtr(), 0, mDummyMeshData);
	}
}