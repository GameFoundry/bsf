#include "CmTransientMesh.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmMeshHeap.h"

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
		return mParentHeap->getVertexData();
	}

	std::shared_ptr<IndexData> TransientMesh::getIndexData() const
	{
		return mParentHeap->getIndexData();
	}

	UINT32 TransientMesh::getVertexOffset() const
	{
		return mParentHeap->getVertexOffset(mId);
	}

	UINT32 TransientMesh::getIndexOffset() const
	{
		return mParentHeap->getIndexOffset(mId);
	}

	void TransientMesh::notifyUsedOnGPU()
	{
		mParentHeap->notifyUsedOnGPU(mId);
	}
}