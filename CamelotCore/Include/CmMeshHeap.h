#pragma once

#include "CmPrerequisites.h"
#include "CmCoreObject.h"
#include "CmIndexData.h"
#include "CmDrawOps.h"

namespace BansheeEngine
{
	/**
	 * @brief	Mesh heap allows you to quickly allocate and deallocate a 
	 * 			large amounts of temporary meshes without the large overhead of normal Mesh creation.
	 * 			Only requirement is that meshes share the same vertex description and index type.
	 * 			
	 * @note	This class should be considered as a replacement for a normal Mesh if you are constantly 
	 * 			updating the mesh (e.g. every frame) and you are not able to discard entire mesh contents 
	 * 			on each update. Not using discard flag on normal meshes may introduce GPU-CPU sync points
	 * 			which may severely limit performance. Primary purpose of this class is to avoid
	 * 			those sync points by not forcing you to discard contents.
	 * 			
	 *			Only downside is that this class may allocate 2-3x (or more) memory than it is actually needed
	 *			for your data.
	 *			
	 *			Sim thread only.
	 */
	class CM_EXPORT MeshHeap : public CoreObject
	{
		enum class UseFlags
		{
			Used,
			CPUFree,
			GPUFree,
			Free
		};

		struct ChunkData
		{
			UINT32 start, size;
		};

		struct AllocatedData
		{
			UINT32 vertChunkIdx;
			UINT32 idxChunkIdx;

			UseFlags useFlags;
			UINT32 eventQueryIdx;
		};

		struct QueryData
		{
			EventQueryPtr query;
			UINT32 queryId;
		};

	public:
		~MeshHeap();

		/**
		 * @note	Offsets provided by MeshData are ignored. MeshHeap will determine
		 * 			where the data will be written internally.
		 */
		TransientMeshPtr alloc(const MeshDataPtr& meshData, DrawOperationType drawOp = DOT_TRIANGLE_LIST);
		void dealloc(const TransientMeshPtr& mesh);

		static MeshHeapPtr create(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

	private:
		UINT32 mNumVertices; // Core thread
		UINT32 mNumIndices; // Core thread

		std::shared_ptr<VertexData> mVertexData; // Core thread
		std::shared_ptr<IndexData> mIndexData; // Core thread

		Vector<UINT8*> mCPUVertexData; // Core thread
		UINT8* mCPUIndexData; // Core thread

		VertexDataDescPtr mVertexDesc; // Immutable
		IndexBuffer::IndexType mIndexType; // Immutable

		Map<UINT32, TransientMeshPtr> mMeshes; // Sim thread
		UINT32 mNextFreeId; // Sim thread

		Map<UINT32, AllocatedData> mMeshAllocData; // Core thread

		Vector<ChunkData> mVertChunks; // Core thread
		Vector<ChunkData> mIdxChunks; // Core thread

		Stack<UINT32> mEmptyVertChunks; // Core thread
		Stack<UINT32> mEmptyIdxChunks; // Core thread

		List<UINT32> mFreeVertChunks; // Core thread
		List<UINT32> mFreeIdxChunks; // Core thread

		Vector<QueryData> mEventQueries; // Core thread
		Stack<UINT32> mFreeEventQueries; // Core thread

		UINT32 mNextQueryId;

		static const float GrowPercent;
	private:
		friend class TransientMesh;

		MeshHeap(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @copydoc Resource::initialize_internal()
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc Resource::destroy_internal()
		 */
		virtual void destroy_internal();

		void allocInternal(UINT32 meshId, const MeshDataPtr& meshData);
		void deallocInternal(UINT32 meshId);

		void growVertexBuffer(UINT32 numVertices);
		void growIndexBuffer(UINT32 numIndices);

		UINT32 createEventQuery();
		void freeEventQuery(UINT32 idx);

		std::shared_ptr<VertexData> getVertexData() const;
		std::shared_ptr<IndexData> getIndexData() const;

		UINT32 getVertexOffset(UINT32 meshId) const;
		UINT32 getIndexOffset(UINT32 meshId) const;

		void notifyUsedOnGPU(UINT32 meshId);
		void queryTriggered(UINT32 meshId, UINT32 queryId);

		void mergeWithNearbyChunks(UINT32 chunkVertIdx, UINT32 chunkIdxIdx);
	};
}