#pragma once

#include "CmPrerequisites.h"
#include "CmMeshBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a single mesh entry in the MeshHeap. This can be used as a normal mesh
	 *			but due to the nature of the mesh-heap it is not the type of mesh you should use
	 *			for storing static data.
	 *
	 *			Transient meshes don't keep internal index/vertex buffers but instead use the ones
	 *			provided by their parent mesh heap.
	 *
	 * @see		MeshHeap
	 */
	class BS_CORE_EXPORT TransientMesh : public MeshBase
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

		/**
		 * @brief	Constructs a new transient mesh.
		 *
		 * @see		MeshHeap::alloc
		 */
		TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numIndices, 
			UINT32 numVertices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Marks the mesh as destroyed so we know that we don't need to destroy it ourselves.
		 */
		void markAsDestroyed() { mIsDestroyed = true; }

	protected:
		bool mIsDestroyed;
		MeshHeapPtr mParentHeap;
		UINT32 mId;
	};
}