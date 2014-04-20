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

		const AABox& getBounds() const;
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
		static HMesh create(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		static HMesh create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		static HMesh create(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialMeshData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		static MeshPtr _createPtr(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);
	};
}