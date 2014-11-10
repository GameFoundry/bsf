#pragma once

#include "BsCorePrerequisites.h"
#include "BsMeshBase.h"
#include "BsMeshData.h"
#include "BsVertexData.h"
#include "BsDrawOps.h"
#include "BsSubMesh.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	/**
	 * @brief	Core thread portion of a Mesh.
	 *
	 * @see		Mesh.
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT MeshCore : public MeshCoreBase
	{
	public:
		MeshCore(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
			const Vector<SubMesh>& subMeshes, MeshBufferType bufferType, IndexType indexType,
			MeshDataPtr initialMeshData);

		/**
		 * @brief	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @brief	CoreObjectCore::destroy
		 */
		virtual void destroy();

		/**
		 * @copydoc MeshCoreBase::getVertexData
		 */
		virtual SPtr<VertexData> getVertexData() const;

		/**
		 * @copydoc MeshCoreBase::getIndexBuffer
		 */
		virtual SPtr<IndexBufferCore> getIndexBuffer() const;

		/**
		 * @brief	Updates a part of the current mesh with the provided data.
		 *
		 * @param	subresourceIdx		Index of the subresource to update, if the mesh has more than one.
		 * @param	data				Data to update the mesh with.
		 * @param	discardEntireBuffer When true the existing contents of the resource you are updating will be discarded. This can make the
		 * 								operation faster. Resources with certain buffer types might require this flag to be in a specific state
		 * 								otherwise the operation will fail.
		 * @param	updateBounds		If true the internal bounds of the mesh will be recalculated based on the provided data.
		 */
		virtual void writeSubresource(UINT32 subresourceIdx, const MeshData& data, bool discardEntireBuffer, bool updateBounds = true);

		/**
		 * @brief	Reads a part of the current resource into the provided "data" parameter.
		 * 			Data buffer needs to be pre-allocated.
		 *
		 * @param	subresourceIdx		Index of the subresource to update, if the mesh has more than one.
		 * @param	data				Buffer that will receive the data. Should be allocated with "allocateSubresourceBuffer"
		 *								to ensure it is of valid type and size.
		 */
		virtual void readSubresource(UINT32 subresourceIdx, MeshData& data);

	protected:
		friend class Mesh;

		/**
		 * @brief	Updates bounds by calculating them from the vertices in the provided mesh data object.
		 */
		void updateBounds(const MeshData& meshData);

		std::shared_ptr<VertexData> mVertexData;
		SPtr<IndexBufferCore> mIndexBuffer;

		VertexDataDescPtr mVertexDesc;
		MeshBufferType mBufferType;
		IndexType mIndexType;
		MeshDataPtr mTempInitialMeshData;
	};

	/**
	 * @brief	Primary class for holding geometry. Stores data in the form of a vertex 
	 *			buffers and optionally index buffer, which may be bound to the pipeline for drawing.
	 *			May contain multiple sub-meshes.
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT Mesh : public MeshBase
	{
	public:
		virtual ~Mesh();

		/**
		 * @copydoc	MeshBase::initialize
		 */
		virtual void initialize();

		/**
		 * @brief	Updates the mesh with new data. The actual write will be queued for later execution on the core thread.
		 *			Provided data buffer will be locked until the operation completes.
		 *
		 * @param	accessor			Accessor to queue the operation on.
		 * 
		 * @return	Async operation object you can use to track operation completion.
		 *
		 * @see		MeshCore::writeSubresource
		 */
		AsyncOp writeSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const MeshDataPtr& data, bool discardEntireBuffer);

		/**
		 * @brief	Reads internal mesh data to the provided previously allocated buffer. The read is queued for execution
		 *			on the core thread and not executed immediately. Provided data buffer will be locked until the
		 *			operation completes.
		 *
		 * @param	accessor			Accessor to queue the operation on.
		 *
		 * @return	Async operation object you can use to track operation completion.
		 *
		 * @see		MeshCore::readSubresource
		 */
		AsyncOp readSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const MeshDataPtr& data);

		/**
		 * @brief	Allocates a buffer you may use for storage when reading a subresource. You
		 * 			need to allocate such a buffer if you are calling "readSubresource".
		 * 			
		 * @param	subresourceIdx	Only 0 is supported. You can only update entire mesh at once.
		 *
		 * @note	Thread safe.
		 */
		MeshDataPtr allocateSubresourceBuffer(UINT32 subresourceIdx) const;

		/**
		 * @brief	Retrieves a core implementation of a mesh usable only from the
		 *			core thread.
		 */
		SPtr<MeshCore> getCore() const;

		/**
		 * @brief	Returns a dummy mesh, containing just one triangle. Don't modify the returned mesh.
		 */
		static HMesh dummy();

	protected:
		friend class MeshManager;

		Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST,
			IndexType indexType = IT_32BIT);

		Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
			const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static,
			IndexType indexType = IT_32BIT);

		Mesh(const MeshDataPtr& initialMeshData, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		Mesh(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static);

		/**
		 * @brief	Updates bounds by calculating them from the vertices in the provided mesh data object.
		 */
		void updateBounds(const MeshData& meshData);

		/**
		 * @copydoc	RenderTarget::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		mutable MeshDataPtr mTempInitialMeshData;

		VertexDataDescPtr mVertexDesc;
		MeshBufferType mBufferType;
		IndexType mIndexType;

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
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT);

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
			MeshBufferType bufferType = MeshBufferType::Static, IndexType indexType = IT_32BIT);

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
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, MeshBufferType, DrawOperationType, IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static,
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT);

		/**
		 * @copydoc	create(UINT32, UINT32, const VertexDataDescPtr&, const Vector<SubMesh>&, MeshBufferType, IndexType)
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static MeshPtr _createPtr(UINT32 numVertices, UINT32 numIndices, 
			const VertexDataDescPtr& vertexDesc, const Vector<SubMesh>& subMeshes,
			MeshBufferType bufferType = MeshBufferType::Static, IndexType indexType = IT_32BIT);

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