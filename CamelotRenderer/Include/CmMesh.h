#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmMeshData.h"
#include "CmVertexIndexData.h"

namespace CamelotEngine
{
	struct SubMesh
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

	class Mesh : public Resource
	{
	public:
		Mesh();
		virtual ~Mesh();

		/**
		 * @brief	Mesh data that is used for initializing the mesh. Needs to be set before calling load.
		 */
		void prepare(MeshDataPtr meshData);

		virtual void loadImpl();

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