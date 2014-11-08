#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"
#include "BsDrawOps.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	Mesh heap allows you to quickly allocate and deallocate a large amounts of temporary 
	 *			meshes without the large overhead of normal Mesh creation.
	 * 			Only requirement is that meshes share the same vertex description and index type.
	 * 			
	 * @note	This class should be considered as a replacement for a normal Mesh if you are constantly 
	 * 			updating the mesh (e.g. every frame) and you are not able to discard entire mesh contents 
	 * 			on each update. Not using discard flag on normal meshes may introduce GPU-CPU sync points
	 * 			which may severely limit performance. Primary purpose of this class is to avoid
	 * 			those sync points by not forcing you to discard contents.
	 * 			
	 *			Downside is that this class may allocate 2-3x (or more) memory than it is actually needed
	 *			for your data.
	 *			
	 *			Sim thread except where noted otherwise.
	 */
	class BS_CORE_EXPORT MeshHeap : public CoreObject
	{
		/**
		 * @brief	Signifies how is a data chunk used.
		 */
		enum class UseFlags
		{
			Used, /**< Data chunk is used by both CPU and GPU. */
			CPUFree, /**< Data chunk was released by CPU but not GPU. */
			GPUFree, /**< Data chunk was released by GPU but not CPU. */
			Free /**< Data chunk was released by both CPU and GPU. */
		};

		/**
		 * @brief	Represents a continuous chunk of memory.
		 */
		struct ChunkData
		{
			UINT32 start, size;
		};

		/**
		 * @brief	Represents an allocated piece of data representing a mesh.
		 */
		struct AllocatedData
		{
			UINT32 vertChunkIdx;
			UINT32 idxChunkIdx;

			UseFlags useFlags;
			UINT32 eventQueryIdx;
			TransientMeshPtr mesh;
		};

		/**
		 * @brief	Data about a GPU query.
		 */
		struct QueryData
		{
			EventQueryPtr query;
			UINT32 queryId;
		};

	public:
		~MeshHeap();

		/**
		 * @brief	Allocates a new mesh in the heap, expanding the heap if needed. Mesh will be initialized
		 *			with the provided meshData. You may use the returned transient mesh for drawing.
		 *
		 * @note	Offsets provided by MeshData are ignored. MeshHeap will determine
		 * 			where the data will be written internally.
		 */
		TransientMeshPtr alloc(const MeshDataPtr& meshData, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Deallocates the provided mesh and makes that room on the heap re-usable as soon as the GPU
		 *			is also done with the mesh.
		 */
		void dealloc(const TransientMeshPtr& mesh);

		/**
		 * @brief	Creates a new mesh heap.
		 *
		 * @param	numVertices	Initial number of vertices the heap may store. This will grow automatically if needed.
		 * @param	numIndices	Initial number of indices the heap may store. This will grow automatically if needed.
		 * @param	vertexDesc	Description of the stored vertices.
		 * @param	indexType	Type of the stored indices.
		 */
		static MeshHeapPtr create(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, IndexType indexType = IT_32BIT);

	private:
		friend class TransientMesh;

		/**
		 * @copydoc	create
		 */
		MeshHeap(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, IndexType indexType = IT_32BIT);

		/**
		 * @copydoc CoreObject::initialize_internal()
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreObject::destroy_internal()
		 */
		virtual void destroy_internal();

		/**
		 * @brief	Allocates a new mesh in the heap, expanding the heap if needed. 
		 *
		 * @param	meshId		Mesh for which we are allocating the data.
		 * @param	meshData	Data to initialize the new mesh with.
		 *
		 * @note	Core thread.
		 */
		void allocInternal(TransientMeshPtr mesh, const MeshDataPtr& meshData);

		/**
		 * @brief	Deallocates the provided mesh Freed memory
		 *			will be re-used as soon as the GPU is done with the mesh
		 *
		 * @note	Core thread.
		 */
		void deallocInternal(TransientMeshPtr mesh);

		/**
		 * @brief	Resizes the vertex buffers so they max contain the provided
		 *			number of vertices.
		 *
		 * @note	Core thread.
		 */
		void growVertexBuffer(UINT32 numVertices);

		/**
		 * @brief	Resizes the index buffer so they max contain the provided
		 *			number of indices.
		 *
		 * @note	Core thread.
		 */
		void growIndexBuffer(UINT32 numIndices);

		/**
		 * @brief	Creates a new event query or returns an existing one from the pool
		 *			if available. Returned value is an index into mEventQueries array.
		 *
		 * @note	Core thread.
		 */
		UINT32 createEventQuery();

		/**
		 * @brief	Frees the event query with the specified index and returns it to the
		 *			pool so it may be reused later.
		 *
		 * @note	Core thread.
		 */
		void freeEventQuery(UINT32 idx);

		/**
		 * @brief	Gets internal vertex data for all the meshes.
		 */
		SPtr<VertexData> getVertexData() const;

		/**
		 * @brief	Gets internal index data for all the meshes.
		 */
		SPtr<IndexBufferCore> getIndexBuffer() const;

		/**
		 * @brief	Returns the offset in vertices from the start of the buffer
		 *			to the first vertex of the mesh with the provided ID.
		 */
		UINT32 getVertexOffset(UINT32 meshId) const;

		/**
		 * @brief	Returns the offset in indices from the start of the buffer
		 *			to the first index of the mesh with the provided ID.
		 */
		UINT32 getIndexOffset(UINT32 meshId) const;

		/**
		 * @brief	Called by the render system when a mesh gets queued to the GPU.
		 */
		void notifyUsedOnGPU(UINT32 meshId);

		/**
		 * @brief	Called by an GPU event query when GPU processes the query. Normally
		 *			signals the heap that the GPU is done with the mesh.
		 */
		static void queryTriggered(MeshHeapPtr thisPtr, UINT32 meshId, UINT32 queryId);

		/**
		 * @brief	Attempts to reorganize the vertex and index buffer chunks in order to 
		 *			in order to make free memory contigous.
		 *
		 * @note	This will not actually copy any data from index/vertex buffers, and will only
		 *			modify the chunk descriptors.
		 */
		void mergeWithNearbyChunks(UINT32 chunkVertIdx, UINT32 chunkIdxIdx);

	private:
		UINT32 mNumVertices; // Core thread
		UINT32 mNumIndices; // Core thread

		Vector<UINT8*> mCPUVertexData; // Core thread
		UINT8* mCPUIndexData; // Core thread

		SPtr<VertexData> mVertexData; // Core thread
		SPtr<IndexBufferCore> mIndexBuffer; // Core thread

		Map<UINT32, AllocatedData> mMeshAllocData; // Core thread

		VertexDataDescPtr mVertexDesc; // Immutable
		IndexType mIndexType; // Immutable

		Map<UINT32, TransientMeshPtr> mMeshes; // Sim thread
		UINT32 mNextFreeId; // Sim thread

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
	};
}