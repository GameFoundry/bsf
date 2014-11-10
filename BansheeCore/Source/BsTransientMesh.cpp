#include "BsTransientMesh.h"
#include "BsVertexData.h"
#include "BsBounds.h"
#include "BsMeshHeap.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	TransientMeshCore::TransientMeshCore(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes)
		:MeshCoreBase(numVertices, numIndices, subMeshes), mParentHeap(parentHeap), mId(id)
	{

	}

	SPtr<VertexData> TransientMeshCore::getVertexData() const
	{
		return mParentHeap->getVertexData();
	}

	SPtr<IndexBufferCore> TransientMeshCore::getIndexBuffer() const
	{
		return mParentHeap->getIndexBuffer();
	}

	UINT32 TransientMeshCore::getVertexOffset() const
	{
		return mParentHeap->getVertexOffset(mId);
	}

	UINT32 TransientMeshCore::getIndexOffset() const
	{
		return mParentHeap->getIndexOffset(mId);
	}

	void TransientMeshCore::_notifyUsedOnGPU()
	{
		mParentHeap->notifyUsedOnGPU(mId);
	}

	TransientMesh::TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
		:MeshBase(numVertices, numIndices, drawOp), mParentHeap(parentHeap), mId(id), mIsDestroyed(false)
	{

	}

	TransientMesh::~TransientMesh()
	{
		if (!mIsDestroyed)
		{
			TransientMeshPtr meshPtr = std::static_pointer_cast<TransientMesh>(getThisPtr());
			mParentHeap->dealloc(meshPtr);
		}
	}

	SPtr<TransientMeshCore> TransientMesh::getCore() const
	{
		return std::static_pointer_cast<TransientMeshCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> TransientMesh::createCore() const
	{
		TransientMeshCore* core = new (bs_alloc<TransientMeshCore>()) TransientMeshCore(
			mParentHeap, mId, mProperties.mNumVertices, mProperties.mNumIndices, mProperties.mSubMeshes);

		SPtr<CoreObjectCore> meshCore = bs_shared_ptr<TransientMeshCore, GenAlloc>(core);
		return meshCore;
	}
}