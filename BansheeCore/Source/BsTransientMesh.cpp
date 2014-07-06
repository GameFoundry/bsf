#include "BsTransientMesh.h"
#include "BsVertexData.h"
#include "BsBounds.h"
#include "BsMeshHeap.h"

namespace BansheeEngine
{
	TransientMesh::TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
		:MeshBase(numVertices, numIndices, drawOp), mParentHeap(parentHeap), mId(id), mIsDestroyed(false)
	{

	}

	TransientMesh::~TransientMesh()
	{
		if(!mIsDestroyed)
		{
			TransientMeshPtr meshPtr = std::static_pointer_cast<TransientMesh>(getThisPtr());
			mParentHeap->dealloc(meshPtr);
		}
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

	MeshProxyPtr TransientMesh::_createProxy(UINT32 subMeshIdx)
	{
		MeshProxyPtr coreProxy = bs_shared_ptr<MeshProxy>();
		coreProxy->mesh = std::static_pointer_cast<MeshBase>(getThisPtr());
		coreProxy->subMesh = mSubMeshes[0];
		coreProxy->submeshIdx = subMeshIdx;

		// Note: Not calculating bounds for transient meshes yet
		// (No particular reason, I just didn't bother)
		coreProxy->bounds = Bounds(); 
		
		return coreProxy;
	}
}