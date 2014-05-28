#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmMesh.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manager that handles creation of Meshes.
	 */
	class BS_CORE_EXPORT MeshManager : public Module<MeshManager>
	{
	public:
		MeshManager();
		~MeshManager();

		/**
		 * @brief	Creates an empty mesh with enough space to store vertex and index data described by the parameters.
		 *
		 * @param	numVertices	Number of vertices in the mesh.
		 * @param	numIndices	Number of indices in the mesh.
		 * @param	vertexDesc	Vertex description that describes how are vertices organized within the vertex buffer(s).
		 * @param	bufferType	Type of buffers to use to store mesh vertex and index data. Specify dynamic if you plan on
		 *						updating the mesh often, or static otherwise.
		 * @param	drawOp		Informs the render system on how to draw the mesh. Determines how are indices and vertices interpreted.
		 * @param	indexType	Type of indexes in the index buffer. Determines size of an index.
		 */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			 DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates an mesh with enough space to store vertex and index data described by the parameters, and initializes
		 *			a portion (or entire) mesh with some data.
		 *
		 * @param	numVertices	Number of vertices in the mesh.
		 * @param	numIndices	Number of indices in the mesh.
		 * @param	vertexDesc	Vertex description that describes how are vertices organized within the vertex buffer(s).
		 * @param	initialData	Initial data to initialize the mesh with. Internal offsets in this object will be used to
		 *						determine where to write the data in the Mesh buffers.
		 * @param	bufferType	Type of buffers to use to store mesh vertex and index data. Specify dynamic if you plan on
		 *						updating the mesh often, or static otherwise.
		 * @param	drawOp		Informs the render system on how to draw the mesh. Determines how are indices and vertices interpreted.
		 * @param	indexType	Type of indexes in the index buffer. Determines size of an index.
		 */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const MeshDataPtr& initialData, 
			MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST, 
			IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);

		/**
		 * @brief	Creates a mesh and initializes it with the provided data. Mesh will be of exact size needed to hold the data.
		 *
		 * @param	initialData	Initial data to initialize the mesh with. Size of the buffers and vertex declaration will be taken
		 *						from this object.
		 * @param	bufferType	Type of buffers to use to store mesh vertex and index data. Specify dynamic if you plan on
		 *						updating the mesh often, or static otherwise.
		 * @param	drawOp		Informs the render system on how to draw the mesh. Determines how are indices and vertices interpreted.
		 */
		MeshPtr create(const MeshDataPtr& initialData, MeshBufferType bufferType = MeshBufferType::Static,
			DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Creates a new empty and uninitialized mesh. You will need to manually initialize the mesh before using it.
		 *	
		 * @note	This should only be used for special cases and is not meant for normal use.
		 */
		MeshPtr createEmpty();

		/**
		 * @brief	Returns some dummy mesh data with one triangle you may use for initializing a mesh.
		 */
		MeshDataPtr getDummyMeshData() const { return mDummyMeshData; }

		/**
		 * @brief	Returns a dummy mesh containing one triangle.
		 */
		HMesh getDummyMesh() const { return mDummyMesh; }

	protected:
		/**
		 * @copydoc		Module::onStartUp
		 */
		virtual void onStartUp();

	private:
		MeshDataPtr mDummyMeshData;
		HMesh mDummyMesh;
	};
}