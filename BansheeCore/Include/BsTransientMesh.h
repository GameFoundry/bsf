#pragma once

#include "BsCorePrerequisites.h"
#include "BsMeshBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	Core thread portion of a transient mesh.
	 *
	 * @see		Transient mesh
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT TransientMeshCore : public MeshCoreBase
	{
	public:
		TransientMeshCore(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices,
			UINT32 numIndices, const Vector<SubMesh>& subMeshes);

		/**
		 * @copydoc MeshBase::getVertexData
		 */
		SPtr<VertexData> getVertexData() const;

		 /**
		  * @copydoc MeshBase::getIndexData
		  */
		SPtr<IndexBufferCore> getIndexBuffer() const;

		/**
		 * @brief	Returns the ID that uniquely identifies this mesh in the parent heap.
		 */
		UINT32 getMeshHeapId() const { return mId; }

		/**
		 * @copydoc MeshBase::getVertexOffset
		 */
		virtual UINT32 getVertexOffset() const;

		 /**
		 * @copydoc MeshBase::getIndexOffset
		 */
		virtual UINT32 getIndexOffset() const;

		 /**
		 * @copydoc MeshBase::notifyUsedOnGPU
		 */
		virtual void _notifyUsedOnGPU();

	protected:
		friend class TransientMesh;

		MeshHeapPtr mParentHeap;
		UINT32 mId;
	};

	/**
	 * @brief	Represents a single mesh entry in the MeshHeap. This can be used as a normal mesh
	 *			but due to the nature of the mesh-heap it is not the type of mesh you should use
	 *			for storing static data.
	 *
	 *			Transient meshes don't keep internal index/vertex buffers but instead use the ones
	 *			provided by their parent mesh heap.
	 *
	 * @see		MeshHeap
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT TransientMesh : public MeshBase
	{
	public:
		virtual ~TransientMesh();

		/**
		 * @brief	Retrieves a core implementation of a mesh usable only from the
		 *			core thread.
		 */
		SPtr<TransientMeshCore> getCore() const;

	protected:
		friend class MeshHeap;

		/**
		 * @brief	Constructs a new transient mesh.
		 *
		 * @see		MeshHeap::alloc
		 */
		TransientMesh(const MeshHeapPtr& parentHeap, UINT32 id, UINT32 numVertices,
			UINT32 numIndices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Marks the mesh as destroyed so we know that we don't need to destroy it ourselves.
		 */
		void markAsDestroyed() { mIsDestroyed = true; }

		/**
		 * @copydoc	RenderTarget::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

	protected:
		bool mIsDestroyed;
		MeshHeapPtr mParentHeap;
		UINT32 mId;
	};
}