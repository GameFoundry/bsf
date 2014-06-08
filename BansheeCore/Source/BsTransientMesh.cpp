#include "BsTransientMesh.h"
#include "BsVertexData.h"
#include "BsMeshHeap.h"

namespace BansheeEngine
{
	TransientMesh::TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
		:MeshBase(numVertices, numIndices, drawOp), mParentHeap(parentHeap), mId(id), mIsDestroyed(false)
	{
		mRenderData = bs_shared_ptr<MeshRenderData>(nullptr, nullptr, SubMesh(), 0, std::bind(&TransientMesh::_notifyUsedOnGPU, this));
	}

	TransientMesh::~TransientMesh()
	{
		if(!mIsDestroyed)
		{
			TransientMeshPtr meshPtr = std::static_pointer_cast<TransientMesh>(getThisPtr());
			mParentHeap->dealloc(meshPtr);
		}

		mRenderData->_markAsInvalid();
	}

	void TransientMesh::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer)
	{
		BS_EXCEPT(InvalidStateException, "Updating is not supported on a transient mesh.");
	}

	void TransientMesh::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		BS_EXCEPT(InvalidStateException, "Reading is not supported on a transient mesh.");
	}

	std::shared_ptr<VertexData> TransientMesh::_getVertexData() const
	{
		return mParentHeap->_getVertexData();
	}

	IndexBufferPtr TransientMesh::_getIndexBuffer() const
	{
		return mParentHeap->_getIndexBuffer();
	}

	UINT32 TransientMesh::_getVertexOffset() const
	{
		return mParentHeap->getVertexOffset(mId);
	}

	UINT32 TransientMesh::_getIndexOffset() const
	{
		return mParentHeap->getIndexOffset(mId);
	}

	void TransientMesh::_notifyUsedOnGPU()
	{
		mParentHeap->notifyUsedOnGPU(mId);
	}

	void TransientMesh::_updateRenderData()
	{
		mRenderData->updateData(mParentHeap->_getVertexData(), mParentHeap->_getIndexBuffer(),
			SubMesh(mParentHeap->getIndexOffset(mId), getNumIndices(), getSubMesh(0).drawOp),
			mParentHeap->getVertexOffset(mId));
	}
}