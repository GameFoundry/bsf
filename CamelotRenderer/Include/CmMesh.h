#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmMeshData.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmRenderOperation.h"

namespace CamelotEngine
{
	struct CM_EXPORT SubMesh
	{
		SubMesh():
			indexOffset(0), indexCount(0)
		{ }

		SubMesh(UINT32 indexOffset, UINT32 indexCount):
			indexOffset(indexOffset), indexCount(indexCount)
		{ }

		UINT32 indexOffset;
		UINT32 indexCount;
	};

	class CM_EXPORT Mesh : public Resource
	{
	public:
		virtual ~Mesh();

		void initialize();

		/**
		 * @brief	Mesh data that is used for initializing the mesh. Needs to be set before calling load.
		 */
		void setMeshData(MeshDataPtr meshData);
		void setMeshData_internal(MeshDataPtr meshData);

		/**
		 * @brief	Gets the mesh data from the GPU. This method is slow so be careful when you call it.
		 */
		MeshDataPtr getMeshData();
		void getMeshData_internal(AsyncOp& asyncOp);

		RenderOperation getRenderOperation(UINT32 subMeshIdx = 0) const;

	protected:
		Mesh();

		VertexData* mVertexData;
		IndexData* mIndexData;

		vector<SubMesh>::type mSubMeshes;

		virtual void initialize_internal();
		void throwIfNotRenderThread() const;

		/**
		 * @copydoc Resource::destroy_internal()
		 */
		virtual void destroy_internal();

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
	private:
		static MeshPtr createEmpty();

		/**
		 * @brief	Returns empty mesh data. (Technically it is not empty,
		 * 			as 0 sized buffers will cause problems, so it contains 3 indices
		 * 			and 1 vertex).
		 */
		static MeshDataPtr getNullMeshData();
		
	public:
		static MeshPtr create();
	};
}