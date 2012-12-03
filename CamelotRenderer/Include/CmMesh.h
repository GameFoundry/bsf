#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmMeshData.h"
#include "CmVertexIndexData.h"
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
		virtual void initialize_internal();

		/**
		 * @brief	Mesh data that is used for initializing the mesh. Needs to be set before calling load.
		 */
		void setMeshData(MeshDataPtr meshData);
		void setMeshData_internal(MeshDataPtr meshData);

		/**
		 * @brief	Gets the mesh data from the GPU. This method is slow so be careful when you call it.
		 */
		MeshDataPtr getMeshData();

		RenderOperation getRenderOperation(UINT32 subMeshIdx = 0) const;

	private:
		Mesh();

		VertexData* mVertexData;
		IndexData* mIndexData;

		vector<SubMesh>::type mSubMeshes;

		void throwIfNotRenderThread() const;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		static MeshPtr createEmpty();

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
	public:
		static MeshPtr create();
	};
}