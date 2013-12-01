#pragma once

#include "CmPrerequisites.h"
#include "CmGpuResource.h"
#include "CmDrawOps.h"
#include "CmSubMesh.h"

namespace CamelotFramework
{
	enum class MeshBufferType
	{
		Static,
		Dynamic
	};

	class CM_EXPORT MeshBase : public GpuResource
	{
	public:
		MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);
		virtual ~MeshBase();

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

		UINT32 getNumVertices() const { return mNumVertices; }
		UINT32 getNumIndices() const { return mNumIndices; }

		/**
		 * @brief	Get vertex data used for rendering.
		 *  
		 * @note	Core thread only. Internal method.
		 */
		virtual std::shared_ptr<VertexData> getVertexData() const = 0;

		/**
		 * @brief	Get index data used for rendering.
		 *  
		 * @note	Core thread only. Internal method.
		 */
		virtual std::shared_ptr<IndexData> getIndexData() const = 0;

		/**
		 * @brief	Returns an offset into the vertex buffers that is returned
		 * 			by getVertexData that signifies where this meshes vertices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual UINT32 getVertexOffset() const { return 0; }

		/**
		 * @brief	Returns an offset into the index buffer that is returned
		 * 			by getIndexData that signifies where this meshes indices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual UINT32 getIndexOffset() const { return 0; }

		/**
		 * @brief	Called whenever this mesh starts being used on the GPU.
		 * 			
		 * @note	Needs to be called after all commands referencing this 
		 * 			mesh have been sent to the GPU.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual void notifyUsedOnGPU() { }

	protected:
		Vector<SubMesh>::type mSubMeshes; // Sim thread
		SubMesh mDefaultSubMesh; // Immutable

		UINT32 mNumVertices; // Immutable
		UINT32 mNumIndices; // Immutable

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	private:
		MeshBase(); // Serialization only

	public:
		friend class MeshBaseRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}