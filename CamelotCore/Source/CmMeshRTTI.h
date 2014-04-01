#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmApplication.h"
#include "CmMesh.h"
#include "CmMeshManager.h"

namespace CamelotFramework
{
	class MeshRTTI : public RTTIType<Mesh, MeshBase, MeshRTTI>
	{
		VertexDataDescPtr getVertexDesc(Mesh* obj) { return obj->mVertexDesc; }
		void setVertexDesc(Mesh* obj, VertexDataDescPtr value) { obj->mVertexDesc = value; }

		IndexBuffer::IndexType& getIndexType(Mesh* obj) { return obj->mIndexType; }
		void setIndexType(Mesh* obj, IndexBuffer::IndexType& value) { obj->mIndexType = value; }

		UINT32& getBufferType(Mesh* obj) { return (UINT32&)obj->mBufferType; }
		void setBufferType(Mesh* obj, UINT32& value) { obj->mBufferType = (MeshBufferType)value; }

		MeshDataPtr getMeshData(Mesh* obj) 
		{ 
			MeshDataPtr meshData = obj->allocateSubresourceBuffer(0);

			GpuResourcePtr sharedMeshPtr = std::static_pointer_cast<GpuResource>(obj->getThisPtr());

			gSyncedCoreAccessor().readSubresource(sharedMeshPtr, 0, meshData);
			gSyncedCoreAccessor().submitToCoreThread(true); // We need the data right away, so execute the context and wait until we get it

			return meshData;
		}

		void setMeshData(Mesh* obj, MeshDataPtr meshData) 
		{ 
			obj->mTempInitialMeshData = meshData;
		}

	public:
		MeshRTTI()
		{
			addReflectablePtrField("mVertexDesc", 0, &MeshRTTI::getVertexDesc, &MeshRTTI::setVertexDesc);

			addPlainField("mIndexType", 1, &MeshRTTI::getIndexType, &MeshRTTI::setIndexType);
			addPlainField("mBufferType", 2, &MeshRTTI::getBufferType, &MeshRTTI::setBufferType);

			addReflectablePtrField("mMeshData", 3, &MeshRTTI::getMeshData, &MeshRTTI::setMeshData);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Mesh* mesh = static_cast<Mesh*>(obj);
			mesh->initialize();
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return MeshManager::instance().createEmpty();
		}

		virtual const String& getRTTIName() 
		{
			static String name = "Mesh";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_Mesh;
		}
	};
}