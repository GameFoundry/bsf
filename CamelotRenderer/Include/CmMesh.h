#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmMeshData.h"
#include "CmVertexIndexData.h"
#include "CmRenderCommand.h"

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
		Mesh();
		virtual ~Mesh();

		/**
		 * @brief	Mesh data that is used for initializing the mesh. Needs to be set before calling load.
		 */
		void setMeshData(MeshDataPtr meshData);

		/**
		 * @brief	Gets the mesh data from the GPU. This method is slow so be careful when you call it.
		 */
		MeshDataPtr getMeshData();

		RenderCommand getRenderOperation(UINT32 subMeshIdx = 0) const;

		virtual void initImpl();

	private:
		MeshDataPtr mMeshData;

		VertexData* mVertexData;
		IndexData* mIndexData;

		vector<SubMesh>::type mSubMeshes;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}