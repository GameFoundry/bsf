#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmMesh.h"

namespace CamelotFramework
{
	class CM_EXPORT MeshManager : public Module<MeshManager>
	{
	public:
		MeshManager();
		~MeshManager();

		MeshPtr create(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);
		MeshPtr create(const MeshDataPtr& initialData, MeshBufferType bufferType = MeshBufferType::Static);
		MeshPtr createEmpty();

		/**
		 * @brief	Returns empty mesh data. (Technically it is not empty,
		 * 			as 0 sized buffers will cause problems, so it contains 3 indices
		 * 			and 1 vertex).
		 */
		MeshDataPtr getDummyMeshData() const { return mDummyMeshData; }

		HMesh getDummyMesh() const { return mDummyMesh; }
	private:
		MeshDataPtr mDummyMeshData;
		HMesh mDummyMesh;

	protected:
		virtual void onStartUp();
	};
}