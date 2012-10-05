#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT MeshData : public IReflectable
	{
	public:
		struct SubMeshData
		{
			SubMeshData():
				indexOffset(0), indexCount(0)
			{ }

			int indexOffset;
			int indexCount;
		};

		MeshData();
		~MeshData();

		Vector3* vertex;
		Vector3* normal;
		Vector3* tangent;
		Vector3* bitangent;
		Vector2* uv0;
		Vector2* uv1;
		int* index;

		int indexCount;
		int vertexCount;

		vector<SubMeshData>::type subMeshes;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshDataST;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}