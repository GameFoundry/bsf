#include "CmMeshManager.h"
#include "CmVector3.h"
#include "CmMesh.h"

namespace CamelotFramework
{
	MeshManager::MeshManager()
	{
		mNullMeshData = MeshDataPtr(CM_NEW(MeshData, GenAlloc) MeshData(), &MemAllocDeleter<MeshData, GenAlloc>::deleter);

		auto indices = mNullMeshData->addIndices32(3);
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;

		auto vertices = mNullMeshData->addPositionsVec3(1);
		vertices[0] = Vector3(0, 0, 0);
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