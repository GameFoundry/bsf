#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmHardwareVertexBuffer.h"

namespace CamelotEngine
{
	class CM_EXPORT MeshData : public IReflectable
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

		struct CM_EXPORT VertexData : public IReflectable
		{
			VertexData(UINT32 vertexCount, UINT32 streamIdx = 0);
			~VertexData();

			Vector3* vertex;
			Color* color;
			Vector3* normal;
			Vector3* tangent;
			Vector3* bitangent;
			Vector2* uv0;
			Vector2* uv1;

			UINT32 vertexCount;
			UINT32 streamIdx;

			/************************************************************************/
			/* 								SERIALIZATION                      		*/
			/************************************************************************/
		public:
			friend class VertexDataST;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		MeshData();
		~MeshData();

		int* index;

		int indexCount;
		int vertexCount;

		VertexDeclarationPtr declaration;
		map<int, std::shared_ptr<VertexData>>::type vertexBuffers;
		vector<SubMeshData>::type subMeshes;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}