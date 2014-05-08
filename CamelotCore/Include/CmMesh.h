#pragma once

#include "CmPrerequisites.h"
#include "CmMeshBase.h"
#include "CmMeshData.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmDrawOps.h"
#include "CmSubMesh.h"

namespace BansheeEngine
{
	/**
	 * @brief	Primary class for holding geometry. Stores data in the form of a vertex 
	 *			buffers and optionally index buffer, which may be bound to the pipeline for drawing.
	 *			May contain multiple sub-meshes.
	 */
	class CM_EXPORT Mesh : public MeshBase
	{
	public:
		virtual ~Mesh();

		/**
		 * @copydoc GpuResource::writeSubresource
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer);

		/**
		 * @copydoc GpuResource::readSubresource
		 */
		virtual void readSubresource(UINT32 subresourceIdx, GpuResourceData& data);

		/**
		 * @brief	Allocates a buffer you may use for storage when reading a subresource. You
		 * 			need to allocate such a buffer if you are calling "readSubresource".
		 * 			
		 * @note	This method is thread safe.
		 */
		MeshDataPtr allocateSubresourceBuffer(UINT32 subresourceIdx) const;

		/**
		 * @brief	TODO - Currently does nothing. But normally it should provide a way to map subresource index to
		 * 			a specific submesh or buffer stream. Right now you can only work with entire mesh at once, not its subsets.
		 */
		void mapFromSubresourceIdx(UINT32 subresourceIdx) const {}

		/**
		 * @brief	TODO - Currently does nothing. But normally it should provide a way to map submesh or stream index to
		 * 			a specific subresource index. Right now you can only work with entire mesh at once, not its subsets.
		 */
		UINT32 mapToSubresourceIdx() const { return 0; }

		/**
		 * @brief	Returns an axis aligned bounding box of the geometry contained in the vertex buffers for all submeshes.
		 */
		const AABox& getBounds() const;

		/**
		 * @brief	Returns an axis aligned bounding box of the geometry contained in the specific sub-mesh.
		 */
		const AABox& getBounds(UINT32 submeshIdx) const;

		/**
		 * @copydoc MeshBase::getVertexData
		 */
		virtual std::shared_ptr<VertexData> getVertexData() const;

		/**
		 * @copydoc MeshBase::getIndexData
		 */
		virtual std::shared_ptr<IndexData> getIndexData() const;

		/**
		 * @brief	Returns a dummy mesh, containing just one triangle. Don't modify the returned mesh.
		 */
		static HMesh dummy();

	protected:
		friend class MeshManager;

		Mesh(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		Mesh(const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		std::shared_ptr<VertexData> mVertexData; // Core thread
		std::shared_ptr<IndexData> mIndexData; // Core thread

		VertexDataDescPtr mVertexDesc; // Immutable
		MeshBufferType mBufferType; // Immutable
		IndexBuffer::IndexType mIndexType; // Immutable

		MeshDataPtr mTempInitialMeshData; // Immutable

		/**
		 * @copydoc Resource::initialize_internal()
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc Resource::destroy_internal()
		 */
		virtual void destroy_internal();

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	private:
		Mesh(); // Serialization only

	public:
		friend class MeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
		
	public:
		/**
		 * @brief	Creates a new empty mesh.
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	vertexDesc		Vertex description structure that describes how are vertices organized in the
		 *							vertex buffer. When binding a mesh to the pipeline you must ensure vertex description
		 *							at least partially matches the input description of the currently bound vertex GPU program.
		 * @param	bufferType		Specify static for buffers you don't plan on updating other reading from often. Otherwise specify
		 *							dynamic. This parameter affects performance.
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 * @param	indexType		Size of indices, use smaller size for better performance, however be careful not to go over
		 *							the number of vertices limited by the size.
		 */
		static HMesh create(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates a new mesh and immediately writes some data to the mesh. This is faster than writing
		 *			the data in a separate step after creation.
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	vertexDesc		Vertex description structure that describes how are vertices organized in the
		 *							vertex buffer. When binding a mesh to the pipeline you must ensure vertex description
		 *							at least partially matches the input description of the currently bound vertex GPU program.
		 * @param	initialMeshData	Vertex and index data used for initializing the mesh. Caller must ensure the data vertex and index buffers
		 *							match the ones in the mesh, however the data might only write to a certain subset of the mesh, it does not
		 *							have to write to all of it.
		 * @param	bufferType		Specify static for buffers you don't plan on updating other reading from often. Otherwise specify
		 *							dynamic. This parameter affects performance.
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 * @param	indexType		Size of indices, use smaller size for better performance, however be careful not to go over
		 *							the number of vertices limited by the size.
		 */
		static HMesh create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates a new mesh from an existing mesh data. Created mesh will match the vertex and index buffers described
		 *			by the mesh data exactly.
		 *
		 * @param	initialMeshData	Vertex and index data used for initializing the mesh. 
		 * @param	bufferType		Specify static for buffers you don't plan on updating other reading from often. Otherwise specify
		 *							dynamic. This parameter affects performance.
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 */
		static HMesh create(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, MeshBufferType, DrawOperationType, IndexBuffer::IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, const MeshDataPtr&, MeshBufferType, DrawOperationType, IndexBuffer::IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @copydoc	create(const MeshDataPtr&, MeshBufferType, DrawOperationType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);
	};
}