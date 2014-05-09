#pragma once

#include "CmPrerequisites.h"
#include "CmGpuResource.h"
#include "CmDrawOps.h"
#include "CmSubMesh.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of buffers used by a mesh. These options usually affect performance and 
	 *			you should specify static if you don't plan on modifying the mesh often,
	 *			otherwise specify dynamic.
	 */
	enum class MeshBufferType
	{
		Static,
		Dynamic
	};

	/**
	 * @brief	Base class all mesh implementations derive from. Meshes hold geometry information,
	 *			normally in the form of one or serveral index or vertex buffers. Different mesh implementations
	 *			might choose to manage those buffers differently.
	 */
	class CM_EXPORT MeshBase : public GpuResource
	{
	public:
		/**
		 * @brief	Constructs a new instance.
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 */
		MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);
		virtual ~MeshBase();

		/**
		 * @brief	Removes all sub-meshes in the mesh. All indices in the mesh will be assumed to
		 *			belong to a single mesh.
		 *
		 * @note	Sim thread only.
		 */
		void clearSubMeshes();

		/**
		 * @brief	Allows you to mark a part of the mesh as a sub-mesh so you may draw that part separately.
		 *
		 * @note	Sim thread only.
		 */
		void addSubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Sets a set of sub-meshes containing data used for rendering a 
		 * 			certain portion of this mesh. Overwrites any previous sub-meshes.
		 * 			
		 * @note	Sim thread only.
		 */
		void setSubMeshes(const Vector<SubMesh>& subMeshes);

		/**
		 * @brief	Retrieves a sub-mesh containing data used for rendering a
		 * 			certain portion of this mesh. If no sub-meshes are specified manually
		 *			a special sub-mesh containing all indices is returned.
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
		virtual std::shared_ptr<VertexData> _getVertexData() const = 0;

		/**
		 * @brief	Get index data used for rendering.
		 *  
		 * @note	Core thread only. Internal method.
		 */
		virtual std::shared_ptr<IndexData> _getIndexData() const = 0;

		/**
		 * @brief	Returns an offset into the vertex buffers that is returned
		 * 			by getVertexData that signifies where this meshes vertices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual UINT32 _getVertexOffset() const { return 0; }

		/**
		 * @brief	Returns an offset into the index buffer that is returned
		 * 			by getIndexData that signifies where this meshes indices
		 * 			begin.
		 * 			
		 * @note	Used when multiple meshes share the same buffers.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual UINT32 _getIndexOffset() const { return 0; }

		/**
		 * @brief	Called whenever this mesh starts being used on the GPU.
		 * 			
		 * @note	Needs to be called after all commands referencing this 
		 * 			mesh have been sent to the GPU.
		 * 			
		 *			Core thread only. Internal method.
		 */
		virtual void _notifyUsedOnGPU() { }

	protected:
		Vector<SubMesh> mSubMeshes; // Sim thread
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