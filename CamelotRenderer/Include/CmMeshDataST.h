#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_EXPORT VertexDataST : public RTTIType<MeshData::VertexData, IReflectable, VertexDataST>
	{
	private:
		CM_SETGET_DATABLOCK_MEMBER(vertex, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(normal, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(tangent, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(bitangent, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(uv0, vertexCount, Vector2, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(uv1, vertexCount, Vector2, MeshData::VertexData)

		CM_SETGET_MEMBER(vertexCount, UINT32, MeshData::VertexData);
	public:
		VertexDataST()
		{
			CM_ADD_DATABLOCKFIELD(vertex, 0, VertexDataST)
			CM_ADD_DATABLOCKFIELD(normal, 1, VertexDataST)
			CM_ADD_DATABLOCKFIELD(tangent, 2, VertexDataST)
			CM_ADD_DATABLOCKFIELD(bitangent, 3, VertexDataST)
			CM_ADD_DATABLOCKFIELD(uv0, 4, VertexDataST)
			CM_ADD_DATABLOCKFIELD(uv1, 5, VertexDataST)

			CM_ADD_PLAINFIELD(vertexCount, 7, VertexDataST)
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData::VertexData>(new MeshData::VertexData(0));
		}

		virtual const String& getRTTIName() 
		{
			static String name = "MeshData::VertexData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return 104;
		}
	};

	class CM_EXPORT MeshDataST : public RTTIType<MeshData, IReflectable, MeshDataST>
	{
	private:
		CM_SETGET_MEMBER(indexCount, INT32, MeshData)
		CM_SETGET_MEMBER(vertexCount, INT32, MeshData);

		CM_SETGET_DATABLOCK_MEMBER(index, indexCount, int, MeshData)

		MeshData::SubMeshData& getSubmesh(MeshData* obj, UINT32 idx)
		{
			return obj->subMeshes[idx];
		}

		void setSubmesh(MeshData* obj, UINT32 idx, MeshData::SubMeshData& data)
		{
			obj->subMeshes[idx] = data;
		}

		UINT32 getSubmeshArraySize(MeshData* obj)
		{
			return obj->subMeshes.size();
		}

		void setSubmeshArraySize(MeshData* obj, UINT32 size)
		{
			obj->subMeshes.resize(size);
		}
	public:
		MeshDataST()
		{
			CM_ADD_DATABLOCKFIELD(index, 0, MeshDataST)

			CM_ADD_PLAINFIELD(indexCount, 1, MeshDataST)
			CM_ADD_PLAINFIELD(vertexCount, 2, MeshDataST)

			addPlainArrayField("subMeshes", 3, &MeshDataST::getSubmesh, &MeshDataST::getSubmeshArraySize, &MeshDataST::setSubmesh, &MeshDataST::setSubmeshArraySize);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData>(new MeshData());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "MeshData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_MeshData;
		}
	};
}