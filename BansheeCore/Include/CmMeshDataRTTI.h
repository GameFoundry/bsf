#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"
#include "CmManagedDataBlock.h"
#include "CmVertexDeclaration.h"

namespace BansheeEngine
{
	CM_ALLOW_MEMCPY_SERIALIZATION(IndexBuffer::IndexType);

	class BS_CORE_EXPORT MeshDataRTTI : public RTTIType<MeshData, GpuResourceData, MeshDataRTTI>
	{
	private:
		VertexDataDescPtr getVertexData(MeshData* obj) { return obj->mVertexData; }
		void setVertexData(MeshData* obj, VertexDataDescPtr value) { obj->mVertexData = value; }

		IndexBuffer::IndexType& getIndexType(MeshData* obj) { return obj->mIndexType; }
		void setIndexType(MeshData* obj, IndexBuffer::IndexType& value) { obj->mIndexType = value; }

		UINT32& getNumVertices(MeshData* obj) { return obj->mNumVertices; }
		void setNumVertices(MeshData* obj, UINT32& value) { obj->mNumVertices = value; }

		UINT32& getNumIndices(MeshData* obj) { return obj->mNumIndices; }
		void setNumIndices(MeshData* obj, UINT32& value) { obj->mNumIndices = value; }

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
			addReflectablePtrField("mVertexData", 0, &MeshDataRTTI::getVertexData, &MeshDataRTTI::setVertexData);

			addPlainField("mIndexType", 1, &MeshDataRTTI::getIndexType, &MeshDataRTTI::setIndexType);
			addPlainField("mNumVertices", 2, &MeshDataRTTI::getNumVertices, &MeshDataRTTI::setNumVertices);
			addPlainField("mNumIndices", 3, &MeshDataRTTI::getNumIndices, &MeshDataRTTI::setNumIndices);

			addDataBlockField("data", 4, &MeshDataRTTI::getData, &MeshDataRTTI::setData, 0, &MeshDataRTTI::allocateData);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return cm_shared_ptr<MeshData, PoolAlloc>(new (cm_alloc<MeshData, PoolAlloc>()) MeshData());
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