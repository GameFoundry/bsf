#include "CmMeshManager.h"
#include "CmVector3.h"
#include "CmMesh.h"

namespace CamelotEngine
{
	MeshManager::MeshManager()
	{
		mNullMeshData = MeshDataPtr(new MeshData());

		mNullMeshData->indexCount = 3;
		mNullMeshData->vertexCount = 1;

		mNullMeshData->index = new int[3];
		mNullMeshData->index[0] = 0;
		mNullMeshData->index[1] = 0;
		mNullMeshData->index[2] = 0;

		std::shared_ptr<MeshData::VertexData> vertexData = std::shared_ptr<MeshData::VertexData>(new MeshData::VertexData(1));
		mNullMeshData->vertexBuffers.insert(std::make_pair(0, vertexData));
		vertexData->vertex = new Vector3[1];
		vertexData->vertex[0] = Vector3(0, 0, 0);

		mNullMeshData->declaration->addElement(0, 0, VET_FLOAT3, VES_POSITION);

		MeshData::SubMeshData subMesh;
		subMesh.indexOffset = 0;
		subMesh.indexCount = 3;
		mNullMeshData->subMeshes.push_back(subMesh);
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
		MeshPtr mesh = MeshPtr(new Mesh());
		mesh->setThisPtr(mesh);

		return mesh;
	}
}