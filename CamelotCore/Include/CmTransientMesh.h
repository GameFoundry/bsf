#pragma once

#include "CmPrerequisites.h"
#include "CmMeshBase.h"

namespace BansheeEngine
{
	class CM_EXPORT TransientMesh : public MeshBase
	{
	public:
		virtual ~TransientMesh();

		/**
		 * @copydoc GpuResource::writeSubresource
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer);

		/**
		 * @copydoc GpuResource::readSubresource
		 */
		virtual void readSubresource(UINT32 subresourceIdx, GpuResourceData& data);

		 /**
		 * @copydoc MeshBase::getVertexData
		 */
		std::shared_ptr<VertexData> _getVertexData() const;

		 /**
		 * @copydoc MeshBase::getIndexData
		 */
		std::shared_ptr<IndexData> _getIndexData() const;

		/**
		 * @copydoc MeshBase::getVertexOffset
		 */
		virtual UINT32 _getVertexOffset() const;

		 /**
		 * @copydoc MeshBase::getIndexOffset
		 */
		virtual UINT32 _getIndexOffset() const;

		 /**
		 * @copydoc MeshBase::notifyUsedOnGPU
		 */
		virtual void _notifyUsedOnGPU();

	protected:
		friend class MeshHeap;

		TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numIndices, 
			UINT32 numVertices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		void markAsDestroyed() { mIsDestroyed = true; }

	protected:
		bool mIsDestroyed;
		MeshHeapPtr mParentHeap;
		UINT32 mId;
	};
}