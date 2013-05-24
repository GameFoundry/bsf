#include "CmMeshManager.h"
#include "CmDeferredRenderContext.h"
#include "CmApplication.h"
#include "CmVector3.h"
#include "CmMesh.h"

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
		MeshPtr mesh = MeshPtr(CM_NEW(Mesh, PoolAlloc) Mesh(), &CoreObject::_deleteDelayed<Mesh, PoolAlloc>);
		mesh->setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::createEmpty()
	{
		MeshPtr mesh = MeshPtr(CM_NEW(Mesh, PoolAlloc) Mesh(), &CoreObject::_deleteDelayed<Mesh, PoolAlloc>);
		mesh->setThisPtr(mesh);

		return mesh;
	}

	void MeshManager::onStartUp()
	{
		mDummyMeshData = MeshDataPtr(CM_NEW(MeshData, GenAlloc) MeshData(1), &MemAllocDeleter<MeshData, GenAlloc>::deleter);

		mDummyMeshData->beginDesc();
		mDummyMeshData->addVertElem(VET_FLOAT3, VES_POSITION);
		mDummyMeshData->addSubMesh(3);
		mDummyMeshData->endDesc();

		auto vecIter = mDummyMeshData->getVec3DataIter(VES_POSITION);
		vecIter.setValue(Vector3(0, 0, 0));

		auto indices = mDummyMeshData->getIndices32(0);
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;

		SyncedRenderContext& renderContext = gMainSyncedRC();

		mDummyMesh = Mesh::create();
		renderContext.writeSubresource(mDummyMesh.getInternalPtr(), 0, *mDummyMeshData);
		renderContext.submitToGpu(true); // TODO - Only temporary until I fix write/read subresource
	}
}