#include "CmTransientMesh.h"
#include "CmVertexData.h"
#include "CmIndexData.h"

namespace CamelotFramework
{
	TransientMesh::TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numIndices, UINT32 numVertices, DrawOperationType drawOp)
		:MeshBase(numVertices, numIndices, drawOp), mParentHeap(parentHeap), mId(id)
	{

	}

	TransientMesh::~TransientMesh()
	{

	}

	void TransientMesh::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer)
	{
		CM_EXCEPT(InvalidStateException, "Updating is not supported on a transient mesh.");
	}

	void TransientMesh::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		CM_EXCEPT(InvalidStateException, "Reading is not supported on a transient mesh.");
	}

	std::shared_ptr<VertexData> TransientMesh::getVertexData() const
	{
		// TODO - Get vertex data from parent MeshHeap
		return nullptr;
	}

	std::shared_ptr<IndexData> TransientMesh::getIndexData() const
	{
		// TODO - Get index data from parent MeshHeap
		return nullptr;
	}

	UINT32 TransientMesh::getVertexOffset() const
	{
		// TODO - Get vertex offset from parent MeshHeap
		return 0;
	}

	UINT32 TransientMesh::getIndexOffset() const
	{
		// TODO - Get index offset from parent MeshHeap
		return 0;
	}

	void TransientMesh::notifyUsedOnGPU()
	{
		// TODO - Refresh EventQuery
	}
}