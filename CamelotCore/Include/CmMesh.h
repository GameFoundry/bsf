#pragma once

#include "CmPrerequisites.h"
#include "CmGpuResource.h"
#include "CmMeshData.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmDrawOps.h"
#include "CmSubMesh.h"

namespace CamelotFramework
{
	enum class MeshBufferType
	{
		Static,
		Dynamic
	};

	class CM_EXPORT Mesh : public GpuResource
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

		void clearSubMeshes();

		void addSubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Sets a set of sub-meshes containing data used for rendering a 
		 * 			certain portion of this mesh. Overwrites any previous sub-meshes.
		 * 			
		 * @note	Sim thread only.
		 */
		void setSubMeshes(const Vector<SubMesh>::type& subMeshes);

		/**
		 * @brief	Retrieves a sub-mesh containing data used for rendering a
		 * 			certain portion of this mesh.
		 * 			
		 * @note	Sim thread only.
		 */
		const SubMesh& getSubMesh(UINT32 subMeshIdx = 0) const;

		/**
		 * @brief	Retrieves a total number of sub-meshes in this mesh.
		 * 			
		 * @note	Sim thread only.
		 */
		UINT32 getNumSubMeshes() const;

		const AABox& getBounds() const;
		const AABox& getBounds(UINT32 submeshIdx) const;

		/**
		 * @brief	Get vertex data used for rendering.
		 *  
		 * @note	Core thread only.
		 */
		std::shared_ptr<VertexData> getVertexData() const;

		/**
		 * @brief	Get index data used for rendering.
		 *  
		 * @note	Core thread only.
		 */
		std::shared_ptr<IndexData> getIndexData() const;

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

		Vector<SubMesh>::type mSubMeshes; // Sim thread
		SubMesh mDefaultSubMesh; // Immutable

		UINT32 mNumVertices; // Immutable
		UINT32 mNumIndices; // Immutable
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
	};
}