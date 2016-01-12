#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsMesh.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Manager that handles creation of Mesh%es. */
	class BS_CORE_EXPORT MeshManager : public Module<MeshManager>
	{
	public:
		MeshManager();
		~MeshManager();

		/** @copydoc Mesh::create(UINT32, UINT32, const VertexDataDescPtr&, int, DrawOperationType, IndexType) */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, int usage = MU_STATIC, 
			DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT);

		/** @copydoc Mesh::create(UINT32, UINT32, const VertexDataDescPtr&, const Vector<SubMesh>&, int, IndexType) */
		MeshPtr create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, const Vector<SubMesh>& subMeshes, 
			int usage = MU_STATIC, IndexType indexType = IT_32BIT);

		/** @copyodc Mesh::create(const MeshDataPtr&, int, DrawOperationType) */
		MeshPtr create(const MeshDataPtr& initialData, int usage = MU_STATIC, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/** @copyodc Mesh::create(const MeshDataPtr&, const Vector<SubMesh>&, int) */
		MeshPtr create(const MeshDataPtr& initialData, const Vector<SubMesh>& subMeshes, int usage = MU_STATIC);

		/**
		 * Creates a new empty and uninitialized mesh. You will need to manually initialize the mesh before using it.
		 *	
		 * @note	This should only be used for special cases and is not meant for normal use.
		 */
		MeshPtr createEmpty();

		/** Returns some dummy mesh data with one triangle you may use for initializing a mesh. */
		MeshDataPtr getDummyMeshData() const { return mDummyMeshData; }

		/**	Returns a dummy mesh containing one triangle. */
		HMesh getDummyMesh() const { return mDummyMesh; }

	protected:
		/** @copydoc Module::onStartUp */
		virtual void onStartUp() override;

	private:
		MeshDataPtr mDummyMeshData;
		HMesh mDummyMesh;
	};

	/** @} */
	/** @endcond */
}