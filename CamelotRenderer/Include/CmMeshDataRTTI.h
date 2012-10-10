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
		CM_SETGET_DATABLOCK_MEMBER(color, vertexCount, Color, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(normal, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(tangent, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(bitangent, vertexCount, Vector3, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(uv0, vertexCount, Vector2, MeshData::VertexData)
		CM_SETGET_DATABLOCK_MEMBER(uv1, vertexCount, Vector2, MeshData::VertexData)

		CM_SETGET_MEMBER(vertexCount, UINT32, MeshData::VertexData);
		CM_SETGET_MEMBER(streamIdx, UINT32, MeshData::VertexData);
	public:
		VertexDataST()
		{
			CM_ADD_DATABLOCKFIELD(vertex, 0, VertexDataST)
			CM_ADD_DATABLOCKFIELD(color, 1, VertexDataST)
			CM_ADD_DATABLOCKFIELD(normal, 2, VertexDataST)
			CM_ADD_DATABLOCKFIELD(tangent, 3, VertexDataST)
			CM_ADD_DATABLOCKFIELD(bitangent, 4, VertexDataST)
			CM_ADD_DATABLOCKFIELD(uv0, 5, VertexDataST)
			CM_ADD_DATABLOCKFIELD(uv1, 6, VertexDataST)

			CM_ADD_PLAINFIELD(vertexCount, 7, VertexDataST)
			CM_ADD_PLAINFIELD(streamIdx, 8, VertexDataST)
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
			return TID_VertexData;
		}
	};

	class CM_EXPORT MeshDataRTTI : public RTTIType<MeshData, IReflectable, MeshDataRTTI>
	{
	private:
		CM_SETGET_MEMBER(indexCount, INT32, MeshData)
		CM_SETGET_MEMBER(vertexCount, INT32, MeshData);

		CM_SETGET_DATABLOCK_MEMBER(index, indexCount, int, MeshData)

		/************************************************************************/
		/* 								subMeshes                      			*/
		/************************************************************************/
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

		/************************************************************************/
		/* 								vertexDeclaration                  		*/
		/************************************************************************/

		VertexDeclaration& getVertexDecl(MeshData* obj) { return obj->declaration; }
		void setVertexDecl(MeshData* obj, VertexDeclaration& vertexDecl) { obj->declaration = vertexDecl; }

		/************************************************************************/
		/* 								vertexData                      		*/
		/************************************************************************/
		std::shared_ptr<MeshData::VertexData> getVertexData(MeshData* obj, UINT32 idx)
		{
			int curIdx = 0;
			for(auto iter = obj->vertexBuffers.begin(); iter != obj->vertexBuffers.end(); ++iter)
			{
				if(curIdx == idx)
					return iter->second;

				curIdx++;
			}

			CM_EXCEPT(InvalidParametersException, "Invalid index: " + toString(idx));
		}

		void setVertexData(MeshData* obj, UINT32 idx, std::shared_ptr<MeshData::VertexData> data)
		{
			obj->vertexBuffers[data->streamIdx] = data;
		}

		UINT32 getVertexDataArraySize(MeshData* obj)
		{
			return obj->vertexBuffers.size();
		}

		void setVertexDataArraySize(MeshData* obj, UINT32 size)
		{
			// Do nothing, map will expand as entries are added
		}
	public:
		MeshDataRTTI()
		{
			CM_ADD_DATABLOCKFIELD(index, 0, MeshDataRTTI)

			CM_ADD_PLAINFIELD(indexCount, 1, MeshDataRTTI)
			CM_ADD_PLAINFIELD(vertexCount, 2, MeshDataRTTI)

			addPlainArrayField("subMeshes", 3, &MeshDataRTTI::getSubmesh, &MeshDataRTTI::getSubmeshArraySize, &MeshDataRTTI::setSubmesh, &MeshDataRTTI::setSubmeshArraySize);
			addReflectableField("vertexDeclaration", 4, &MeshDataRTTI::getVertexDecl, &MeshDataRTTI::setVertexDecl);
			addReflectablePtrArrayField("vertexBuffer", 5, &MeshDataRTTI::getVertexData, &MeshDataRTTI::getVertexDataArraySize, 
				&MeshDataRTTI::setVertexData, &MeshDataRTTI::setVertexDataArraySize);
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