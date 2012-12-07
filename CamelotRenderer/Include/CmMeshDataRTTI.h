#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_EXPORT VertexDataRTTI : public RTTIType<MeshData::VertexData, IReflectable, VertexDataRTTI>
	{
	private:
		ManagedDataBlock getVertex(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->vertex, obj->vertexCount * sizeof(Vector3), false); }	
		void setVertex(MeshData::VertexData* obj, ManagedDataBlock val) { obj->vertex = (Vector3*)val.getData(); } 

		ManagedDataBlock getColor(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->color, obj->vertexCount * sizeof(Color), false); }	
		void setColor(MeshData::VertexData* obj, ManagedDataBlock val) { obj->color = (Color*)val.getData(); } 

		ManagedDataBlock getNormal(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->normal, obj->vertexCount * sizeof(Vector3), false); }	
		void setNormal(MeshData::VertexData* obj, ManagedDataBlock val) { obj->normal = (Vector3*)val.getData(); } 

		ManagedDataBlock getTangent(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->tangent, obj->vertexCount * sizeof(Vector3), false); }	
		void setTangent(MeshData::VertexData* obj, ManagedDataBlock val) { obj->tangent = (Vector3*)val.getData(); } 

		ManagedDataBlock getBitangent(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->bitangent, obj->vertexCount * sizeof(Vector3), false); }	
		void setBitangent(MeshData::VertexData* obj, ManagedDataBlock val) { obj->bitangent = (Vector3*)val.getData(); } 

		ManagedDataBlock getUV0(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->uv0, obj->vertexCount * sizeof(Vector2), false); }	
		void setUV0(MeshData::VertexData* obj, ManagedDataBlock val) { obj->uv0 = (Vector2*)val.getData(); } 

		ManagedDataBlock getUV1(MeshData::VertexData* obj) { return ManagedDataBlock((UINT8*)obj->uv1, obj->vertexCount * sizeof(Vector2), false); }	
		void setUV1(MeshData::VertexData* obj, ManagedDataBlock val) { obj->uv1 = (Vector2*)val.getData(); } 

		CM_SETGET_MEMBER(vertexCount, UINT32, MeshData::VertexData);
		CM_SETGET_MEMBER(streamIdx, UINT32, MeshData::VertexData);
	public:
		VertexDataRTTI()
		{
			addDataBlockField("vertex", 0, &VertexDataRTTI::getVertex, &VertexDataRTTI::setVertex);
			addDataBlockField("color", 1, &VertexDataRTTI::getColor, &VertexDataRTTI::setColor);
			addDataBlockField("normal", 2, &VertexDataRTTI::getNormal, &VertexDataRTTI::setNormal);
			addDataBlockField("tangent", 3, &VertexDataRTTI::getTangent, &VertexDataRTTI::setTangent);
			addDataBlockField("bitangent", 4, &VertexDataRTTI::getBitangent, &VertexDataRTTI::setBitangent);
			addDataBlockField("uv0", 5, &VertexDataRTTI::getUV0, &VertexDataRTTI::setUV0);
			addDataBlockField("uv1", 6, &VertexDataRTTI::getUV1, &VertexDataRTTI::setUV1);

			CM_ADD_PLAINFIELD(vertexCount, 7, VertexDataRTTI)
			CM_ADD_PLAINFIELD(streamIdx, 8, VertexDataRTTI)
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

		ManagedDataBlock getIndex(MeshData* obj) { return ManagedDataBlock((UINT8*)obj->index, obj->indexCount * sizeof(int), false); }	
		void setIndex(MeshData* obj, ManagedDataBlock val) { obj->index = (int*)val.getData(); } 

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
			return (UINT32)obj->subMeshes.size();
		}

		void setSubmeshArraySize(MeshData* obj, UINT32 size)
		{
			obj->subMeshes.resize(size);
		}

		/************************************************************************/
		/* 								vertexDeclaration                  		*/
		/************************************************************************/

		VertexDeclarationPtr getVertexDecl(MeshData* obj) { return obj->declaration; }
		void setVertexDecl(MeshData* obj, VertexDeclarationPtr vertexDecl) { obj->declaration = vertexDecl; }

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
			return (UINT32)obj->vertexBuffers.size();
		}

		void setVertexDataArraySize(MeshData* obj, UINT32 size)
		{
			// Do nothing, map will expand as entries are added
		}
	public:
		MeshDataRTTI()
		{
			addDataBlockField("index", 0, &MeshDataRTTI::getIndex, &MeshDataRTTI::setIndex);

			CM_ADD_PLAINFIELD(indexCount, 1, MeshDataRTTI)
			CM_ADD_PLAINFIELD(vertexCount, 2, MeshDataRTTI)

			addPlainArrayField("subMeshes", 3, &MeshDataRTTI::getSubmesh, &MeshDataRTTI::getSubmeshArraySize, &MeshDataRTTI::setSubmesh, &MeshDataRTTI::setSubmeshArraySize);
			addReflectablePtrField("vertexDeclaration", 4, &MeshDataRTTI::getVertexDecl, &MeshDataRTTI::setVertexDecl);
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