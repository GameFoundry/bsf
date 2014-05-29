#include "BsMeshManager.h"
#include "BsCoreThreadAccessor.h"
#include "BsCoreApplication.h"
#include "BsVector3.h"
#include "BsMesh.h"
#include "BsVertexDataDesc.h"

namespace BansheeEngine
{
	MeshManager::MeshManager()
	{

	}

	MeshManager::~MeshManager()
	{

	}

	MeshPtr MeshManager::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		MeshPtr mesh = bs_core_ptr<Mesh, PoolAlloc>(new (bs_alloc<Mesh, PoolAlloc>()) 
			Mesh(numVertices, numIndices, vertexDesc, bufferType, drawOp, indexType));
		mesh->_setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		const MeshDataPtr& initialData, MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		MeshPtr mesh = bs_core_ptr<Mesh, PoolAlloc>(new (bs_alloc<Mesh, PoolAlloc>()) 
			Mesh(numVertices, numIndices, vertexDesc, initialData, bufferType, drawOp, indexType));
		mesh->_setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::create(const MeshDataPtr& initialData, MeshBufferType bufferType, DrawOperationType drawOp)
	{
		MeshPtr mesh = bs_core_ptr<Mesh, PoolAlloc>(new (bs_alloc<Mesh, PoolAlloc>()) Mesh(initialData, bufferType, drawOp));
		mesh->_setThisPtr(mesh);
		mesh->initialize();

		return mesh;
	}

	MeshPtr MeshManager::createEmpty()
	{
		MeshPtr mesh = bs_core_ptr<Mesh, PoolAlloc>(new (bs_alloc<Mesh, PoolAlloc>()) Mesh());
		mesh->_setThisPtr(mesh);

		return mesh;
	}

	void MeshManager::onStartUp()
	{
		VertexDataDescPtr vertexDesc = bs_shared_ptr<VertexDataDesc>();
		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		mDummyMeshData = bs_shared_ptr<MeshData>(1, 3, vertexDesc);

		auto vecIter = mDummyMeshData->getVec3DataIter(VES_POSITION);
		vecIter.setValue(Vector3(0, 0, 0));

		auto indices = mDummyMeshData->getIndices32();
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;

		mDummyMesh = Mesh::create(mDummyMeshData);
	}
}