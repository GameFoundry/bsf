#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCoreApplication.h"
#include "BsMesh.h"
#include "BsMeshManager.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	class MeshRTTI : public RTTIType<Mesh, MeshBase, MeshRTTI>
	{
		VertexDataDescPtr getVertexDesc(Mesh* obj) { return obj->mVertexDesc; }
		void setVertexDesc(Mesh* obj, VertexDataDescPtr value) { obj->mVertexDesc = value; }

		IndexType& getIndexType(Mesh* obj) { return obj->mIndexType; }
		void setIndexType(Mesh* obj, IndexType& value) { obj->mIndexType = value; }

		int& getBufferType(Mesh* obj) { return (int&)obj->mUsage; }
		void setBufferType(Mesh* obj, int& value) { obj->mUsage = value; }

		MeshDataPtr getMeshData(Mesh* obj) 
		{ 
			MeshDataPtr meshData = obj->allocateSubresourceBuffer(0);

			obj->readSubresource(gCoreAccessor(), 0, meshData);
			gCoreAccessor().submitToCoreThread(true);

			return meshData;
		}

		void setMeshData(Mesh* obj, MeshDataPtr meshData) 
		{ 
			obj->mCPUData = meshData;
		}

	public:
		MeshRTTI()
		{
			addReflectablePtrField("mVertexDesc", 0, &MeshRTTI::getVertexDesc, &MeshRTTI::setVertexDesc);

			addPlainField("mIndexType", 1, &MeshRTTI::getIndexType, &MeshRTTI::setIndexType);
			addPlainField("mUsage", 2, &MeshRTTI::getBufferType, &MeshRTTI::setBufferType);

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