#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsMeshData.h"
#include "BsManagedDataBlock.h"
#include "BsVertexDeclaration.h"

namespace BansheeEngine
{
	BS_ALLOW_MEMCPY_SERIALIZATION(IndexType);

	class BS_CORE_EXPORT MeshDataRTTI : public RTTIType<MeshData, GpuResourceData, MeshDataRTTI>
	{
	private:
		VertexDataDescPtr getVertexData(MeshData* obj) { return obj->mVertexData; }
		void setVertexData(MeshData* obj, VertexDataDescPtr value) { obj->mVertexData = value; }

		IndexType& getIndexType(MeshData* obj) { return obj->mIndexType; }
		void setIndexType(MeshData* obj, IndexType& value) { obj->mIndexType = value; }

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
			return bs_shared_ptr<MeshData, PoolAlloc>(new (bs_alloc<MeshData, PoolAlloc>()) MeshData());
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