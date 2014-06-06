#pragma once

#include "BsCorePrerequisites.h"
#include "BsMeshBase.h"
#include "BsMeshRenderData.h"
#include "BsMeshData.h"
#include "BsVertexData.h"
#include "BsDrawOps.h"
#include "BsSubMesh.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	/**
	 * @brief	Primary class for holding geometry. Stores data in the form of a vertex 
	 *			buffers and optionally index buffer, which may be bound to the pipeline for drawing.
	 *			May contain multiple sub-meshes.
	 */
	class BS_CORE_EXPORT Mesh : public MeshBase
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
		 * @param	subresourceIdx	Only 0 is supported. You can only update entire mesh at once.
		 *
		 * @note	This method is thread safe.
		 */
		MeshDataPtr allocateSubresourceBuffer(UINT32 subresourceIdx) const;

		/**
		 * @brief	Returns bounds of the geometry contained in the vertex buffers for all sub-meshes.
		 */
		const Bounds& getBounds() const { return mBounds; }

		/**
		 * @copydoc MeshBase::getVertexData
		 */
		virtual std::shared_ptr<VertexData> _getVertexData() const;

		/**
		 * @copydoc MeshBase::getIndexData
		 */
		virtual IndexBufferPtr _getIndexBuffer() const;

		/**
		 * @copydoc	MeshBase::_getRenderData
		 */
		MeshRenderData& _getRenderData(UINT32 subMeshIdx) { return mRenderData[subMeshIdx]; }

		/**
		 * @brief	Returns a dummy mesh, containing just one triangle. Don't modify the returned mesh.
		 */
		static HMesh dummy();

	protected:
		friend class MeshManager;

		Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST,
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
			const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static,
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		Mesh(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		Mesh(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static);

		std::shared_ptr<VertexData> mVertexData; // Core thread
		IndexBufferPtr mIndexBuffer; // Core thread

		Bounds mBounds; // Core thread
		Vector<MeshRenderData> mRenderData; // Core thread

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

		/**
		 * @brief	Calculates bounds surrounding the vertices in the provided buffer.
		 *
		 * @param	verticesPtr	Pointer to the buffer containing the positions of vertices to calculate bounds for.
		 * @param	numVertices	Number of vertices in the provided buffer.
		 * @param	stride		How many bytes are needed to advance from one vertex to another.
		 */
		Bounds calculateBounds(UINT8* verticesPtr, UINT32 numVertices, UINT32 stride) const;

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
		 * @brief	Creates a new empty mesh. Created mesh will have no sub-meshes.
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
		static HMesh create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static,
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates a new empty mesh. Created mesh will have specified sub-meshes you may render independently.
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	vertexDesc		Vertex description structure that describes how are vertices organized in the
		 *							vertex buffer. When binding a mesh to the pipeline you must ensure vertex description
		 *							at least partially matches the input description of the currently bound vertex GPU program.
		 * @param	subMeshes		Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered.
		 *							Sub-meshes may be rendered independently.
		 * @param	bufferType		Specify static for buffers you don't plan on updating other reading from often. Otherwise specify
		 *							dynamic. This parameter affects performance.
		 * @param	indexType		Size of indices, use smaller size for better performance, however be careful not to go over
		 *							the number of vertices limited by the size.
		 */
		static HMesh create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const Vector<SubMesh>& subMeshes,
			MeshBufferType bufferType = MeshBufferType::Static, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates a new mesh from an existing mesh data. Created mesh will match the vertex and index buffers described
		 *			by the mesh data exactly. Created mesh will have no sub-meshes.
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
		 * @brief	Creates a new mesh from an existing mesh data. Created mesh will match the vertex and index buffers described
		 *			by the mesh data exactly. Created mesh will have specified sub-meshes you may render independently.
		 *
		 * @param	initialMeshData	Vertex and index data used for initializing the mesh. 
		 * @param	subMeshes		Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered.
		 *							Sub-meshes may be rendered independently.
		 * @param	bufferType		Specify static for buffers you don't plan on updating other reading from often. Otherwise specify
		 *							dynamic. This parameter affects performance.
		 */
		static HMesh create(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static);

		/**
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, MeshBufferType, DrawOperationType, IndexBuffer::IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static,
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, const Vector<SubMesh>&, MeshBufferType, IndexBuffer::IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, const Vector<SubMesh>& subMeshes,
			MeshBufferType bufferType = MeshBufferType::Static, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @copydoc	create(const MeshDataPtr&, MeshBufferType, DrawOperationType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static,
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @copydoc	create(const MeshDataPtr&, const Vector<SubMesh>&, MeshBufferType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes,
			MeshBufferType bufferType = MeshBufferType::Static);
	};
}