#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsMesh.h"

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
		 * @copydoc	Mesh::create(UINT32, UINT32, const VertexDataDescPtr&, MeshBufferType, DrawOperationType, IndexType)
		 */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType = MeshBufferType::Static, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT);

		/**
		 * @copydoc	Mesh::create(UINT32, UINT32, const VertexDataDescPtr&, const Vector<SubMesh>&, MeshBufferType, IndexType)
		 */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const Vector<SubMesh>& subMeshes, 
			MeshBufferType bufferType = MeshBufferType::Static, IndexType indexType = IT_32BIT);

		/**
		 * @copyodc	Mesh::create(const MeshDataPtr&, MeshBufferType, DrawOperationType)
		 */
		MeshPtr create(const MeshDataPtr& initialData, MeshBufferType bufferType = MeshBufferType::Static, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @copyodc	Mesh::create(const MeshDataPtr&, const Vector<SubMesh>&, MeshBufferType)
		 */
		MeshPtr create(const MeshDataPtr& initialData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType = MeshBufferType::Static);

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