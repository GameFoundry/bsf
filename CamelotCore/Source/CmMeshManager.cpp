#include "CmMeshManager.h"
#include "CmVector3.h"
#include "CmMesh.h"

namespace CamelotEngine
{
	MeshManager::MeshManager()
	{
		mNullMeshData = MeshDataPtr(CM_NEW(MeshData, GenAlloc) MeshData(), &MemAllocDeleter<MeshData, GenAlloc>::deleter);

		auto indices = new UINT32[3];
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;

		auto vertices = new Vector3[1];
		vertices[0] = Vector3(0, 0, 0);

		mNullMeshData->setPositions(vertices, 3);
		mNullMeshData->setIndices(indices, 3);
	}

	MeshManager::~MeshManager()
	{

	}

	MeshPtr MeshManager::create()
	{
		MeshPtr mesh = MeshPtr(new Mesh(), &CoreObject::_deleteDelayed);
		mesh->setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::createEmpty()
	{
		MeshPtr mesh = MeshPtr(new Mesh(), &CoreObject::_deleteDelayed);
		mesh->setThisPtr(mesh);

		return mesh;
	}
}