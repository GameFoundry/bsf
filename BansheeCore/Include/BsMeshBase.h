#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuResource.h"
#include "BsBounds.h"
#include "BsDrawOps.h"
#include "BsSubMesh.h"
#include "BsMeshProxy.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of mesh dirty flags
	 */
	enum class MeshDirtyFlag
	{
		Mesh = 0x01, /**< Internal mesh data is dirty. */
		Proxy = 0x02 /**< Active proxy needs to be updated. */
	};


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
	 *
	 * @note	Core thread only unless noted otherwise.
	 */
	class BS_CORE_EXPORT MeshBase : public GpuResource
	{
	public:
		/**
		 * @brief	Constructs a new mesh with no sub-meshes.
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh. 
		 * @param	drawOp			Determines how should the provided indices be interpreted by the pipeline. Default option is triangles,
		 *							where three indices represent a single triangle.
		 */
		MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Constructs a new mesh with one or multiple sub-meshes. (When using just one sub-mesh it is equivalent
		 *			to using the other overload).
		 *
		 * @param	numVertices		Number of vertices in the mesh.
		 * @param	numIndices		Number of indices in the mesh.
		 * @param	subMeshes		Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered.
		 */
		MeshBase(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);

		virtual ~MeshBase();

		/**
		 * @brief	Retrieves a sub-mesh containing data used for rendering a
		 * 			certain portion of this mesh. If no sub-meshes are specified manually
		 *			a special sub-mesh containing all indices is returned.
		 *
		 * @note	Thread safe.
		 */
		const SubMesh& getSubMesh(UINT32 subMeshIdx = 0) const;

		/**
		 * @brief	Retrieves a total number of sub-meshes in this mesh.
		 *
		 * @note	Thread safe.
		 */
		UINT32 getNumSubMeshes() const;

		/**
		 * @brief	Returns maximum number of vertices the mesh may store.
		 *
		 * @note	Thread safe.
		 */
		UINT32 getNumVertices() const { return mNumVertices; }

		/**
		 * @brief	Returns maximum number of indices the mesh may store.
		 *
		 * @note	Thread safe.
		 */
		UINT32 getNumIndices() const { return mNumIndices; }

		/**
		 * @brief	Get vertex data used for rendering.
		 *  
		 * @note	Core thread only. Internal method.
		 */
		virtual SPtr<VertexData> _getVertexData() const = 0;

		/**
		 * @brief	Get index data used for rendering.
		 *  
		 * @note	Core thread only. Internal method.
		 */
		virtual SPtr<IndexBufferCore> _getIndexBuffer() const = 0;

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

		/************************************************************************/
		/* 								CORE PROXY                      		*/
		/************************************************************************/

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and core thread potentially needs to be notified.
		 *
		 * @note	Sim thread only.
		 */
		bool _isCoreDirty(MeshDirtyFlag flag) const { return (mCoreDirtyFlags & (UINT32)flag) != 0; }

		/**
		 * @brief	Marks the core dirty flag as clean.
		 *
		 * @note	Sim thread only.
		 */
		void _markCoreClean(MeshDirtyFlag flag) { mCoreDirtyFlags &= ~(UINT32)flag; }

		/**
		 * @brief	Gets the currently active proxy of this material.
		 */
		MeshProxyPtr _getActiveProxy(UINT32 i) const { return mActiveProxies[i]; }

		/**
		 * @brief	Sets an active proxy for this material.
		 */
		void _setActiveProxy(UINT32 i, const MeshProxyPtr& proxy) { mActiveProxies[i] = proxy; }

		/**
		 * @brief	Creates a new core proxy from the current mesh data. Core proxy contains a snapshot of 
		 *			mesh data normally managed on the sim thread (e.g. bounds).
		 *
		 * @param	subMeshIdx	Index of the sub-mesh to create a proxy for.
		 *
		 * @note	Sim thread only. 
		 *			You generally need to update the core thread with a new proxy whenever core 
		 *			dirty flag is set.
		 */
		virtual MeshProxyPtr _createProxy(UINT32 subMeshIdx) = 0;

	protected:
		/**
		 * @brief	Marks the core data as dirty.
		 */
		void markCoreDirty() { mCoreDirtyFlags = 0xFFFFFFFF; }

	protected:
		Vector<SubMesh> mSubMeshes; // Immutable
		UINT32 mNumVertices; // Immutable
		UINT32 mNumIndices; // Immutable
		Vector<MeshProxyPtr> mActiveProxies;

		UINT32 mCoreDirtyFlags;

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