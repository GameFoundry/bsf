//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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

		IndexBuffer::IndexType& getIndexType(Mesh* obj) { return obj->mIndexType; }
		void setIndexType(Mesh* obj, IndexBuffer::IndexType& value) { obj->mIndexType = value; }

		UINT32& getBufferType(Mesh* obj) { return (UINT32&)obj->mBufferType; }
		void setBufferType(Mesh* obj, UINT32& value) { obj->mBufferType = (MeshBufferType)value; }

		MeshDataPtr getMeshData(Mesh* obj) 
		{ 
			MeshDataPtr meshData = obj->allocateSubresourceBuffer(0);
			GpuResourcePtr sharedMeshPtr = std::static_pointer_cast<GpuResource>(obj->getThisPtr());

			meshData->_lock();
			gCoreThread().queueReturnCommand(std::bind(&RenderSystem::readSubresource, RenderSystem::instancePtr(), 
				sharedMeshPtr, 0, std::static_pointer_cast<GpuResourceData>(meshData), std::placeholders::_1), true);

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