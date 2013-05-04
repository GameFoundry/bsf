#include "CmMeshManager.h"
#include "CmVector3.h"
#include "CmMesh.h"

namespace CamelotFramework
{
	MeshManager::MeshManager()
	{
		mNullMeshData = MeshDataPtr(CM_NEW(MeshData, GenAlloc) MeshData(1), &MemAllocDeleter<MeshData, GenAlloc>::deleter);

		mNullMeshData->beginDesc();
		mNullMeshData->addVertElem(VET_FLOAT3, VES_POSITION);
		mNullMeshData->addSubMesh(3);
		mNullMeshData->endDesc();

		auto vecIter = mNullMeshData->getVec3DataIter(VES_POSITION);
		vecIter.setValue(Vector3(0, 0, 0));

		auto indices = mNullMeshData->getIndices32(0);
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;
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
}