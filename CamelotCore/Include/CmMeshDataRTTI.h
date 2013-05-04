#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"
#include "CmManagedDataBlock.h"
#include "CmVertexDeclaration.h"

namespace CamelotFramework
{
	CM_ALLOW_MEMCPY_SERIALIZATION(MeshData::IndexElementData);
	CM_ALLOW_MEMCPY_SERIALIZATION(IndexBuffer::IndexType);

	class CM_EXPORT MeshDataRTTI : public RTTIType<MeshData, IReflectable, MeshDataRTTI>
	{
	private:
		VertexElement& getVertexElementData(MeshData* obj, UINT32 arrayIdx)
		{
			return obj->mVertexElements[arrayIdx];
		}

		void setVertexElementData(MeshData* obj, UINT32 arrayIdx, VertexElement& value)
		{
			obj->mVertexElements[arrayIdx] = value;
		}

		UINT32 getNumVertexElementData(MeshData* obj)
		{
			return (UINT32)obj->mVertexElements.size();
		}

		void setNumVertexElementData(MeshData* obj, UINT32 numElements)
		{
			obj->mVertexElements.resize(numElements);
		}

		MeshData::IndexElementData& getIndexElementData(MeshData* obj, UINT32 arrayIdx)
		{
			return obj->mSubMeshes[arrayIdx];
		}

		void setIndexElementData(MeshData* obj, UINT32 arrayIdx, MeshData::IndexElementData& value)
		{
			obj->mSubMeshes[arrayIdx] = value;
		}

		UINT32 getNumIndexElementData(MeshData* obj)
		{
			return (UINT32)obj->mSubMeshes.size();
		}

		void setNumIndexElementData(MeshData* obj, UINT32 numElements)
		{
			obj->mSubMeshes.resize(numElements);
		}

		IndexBuffer::IndexType& getIndexType(MeshData* obj)
		{
			return obj->mIndexType;
		}

		void setIndexType(MeshData* obj, IndexBuffer::IndexType& value)
		{
			obj->mIndexType = value;
		}

		UINT32& getNumVertices(MeshData* obj)
		{
			return obj->mNumVertices;
		}

		void setNumVertices(MeshData* obj, UINT32& value)
		{
			obj->mNumVertices = value;
		}

		ManagedDataBlock getData(MeshData* obj) 
		{ 
			ManagedDataBlock dataBlock((UINT8*)obj->getData(), obj->getInternalBufferSize());
			return dataBlock; 
		}

		void setData(MeshData* obj, ManagedDataBlock val) 
		{ 
			// Nothing to do here, the pointer we provided already belongs to PixelData
			// so the data is already written
		}

		static UINT8* allocateData(MeshData* obj, UINT32 numBytes)
		{
			obj->allocateInternalBuffer(numBytes);

			return obj->getData();
		}

	public:
		MeshDataRTTI()
		{
			addPlainArrayField("mVertexData", 0, &MeshDataRTTI::getVertexElementData, 
				&MeshDataRTTI::getNumVertexElementData, &MeshDataRTTI::setVertexElementData, &MeshDataRTTI::setNumVertexElementData);

			addPlainArrayField("mIndexBuffer", 1, &MeshDataRTTI::getIndexElementData, 
				&MeshDataRTTI::getNumIndexElementData, &MeshDataRTTI::setIndexElementData, &MeshDataRTTI::setNumIndexElementData);

			addPlainField("mIndexType", 2, &MeshDataRTTI::getIndexType, &MeshDataRTTI::setIndexType);
			addPlainField("mNumVertices", 3, &MeshDataRTTI::getNumVertices, &MeshDataRTTI::setNumVertices);

			addDataBlockField("data", 4, &MeshDataRTTI::getData, &MeshDataRTTI::setData, 0, &MeshDataRTTI::allocateData);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData>(CM_NEW(MeshData, PoolAlloc) MeshData(),
				&MemAllocDeleter<MeshData, PoolAlloc>::deleter);
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