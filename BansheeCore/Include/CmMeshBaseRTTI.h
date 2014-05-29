#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "BsCoreApplication.h"
#include "CmMeshBase.h"
#include "CmException.h"

namespace BansheeEngine
{
	CM_ALLOW_MEMCPY_SERIALIZATION(SubMesh);

	class MeshBaseRTTI : public RTTIType<MeshBase, GpuResource, MeshBaseRTTI>
	{
		SubMesh& getSubMesh(MeshBase* obj, UINT32 arrayIdx) { return obj->mSubMeshes[arrayIdx]; }
		void setSubMesh(MeshBase* obj, UINT32 arrayIdx, SubMesh& value) { obj->mSubMeshes[arrayIdx] = value; }
		UINT32 getNumSubmeshes(MeshBase* obj) { return (UINT32)obj->mSubMeshes.size(); }
		void setNumSubmeshes(MeshBase* obj, UINT32 numElements) { obj->mSubMeshes.resize(numElements); }

		SubMesh& getDefaultSubMesh(MeshBase* obj) { return obj->mDefaultSubMesh; }
		void setDefaultSubMesh(MeshBase* obj, SubMesh& value) { obj->mDefaultSubMesh = value; }

		UINT32& getNumVertices(MeshBase* obj) { return obj->mNumVertices; }
		void setNumVertices(MeshBase* obj, UINT32& value) { obj->mNumVertices = value; }

		UINT32& getNumIndices(MeshBase* obj) { return obj->mNumIndices; }
		void setNumIndices(MeshBase* obj, UINT32& value) { obj->mNumIndices = value; }

	public:
		MeshBaseRTTI()
		{
			addPlainField("mNumVertices", 0, &MeshBaseRTTI::getNumVertices, &MeshBaseRTTI::setNumVertices);
			addPlainField("mNumIndices", 1, &MeshBaseRTTI::getNumIndices, &MeshBaseRTTI::setNumIndices);

			addPlainArrayField("mSubMeshes", 2, &MeshBaseRTTI::getSubMesh, 
				&MeshBaseRTTI::getNumSubmeshes, &MeshBaseRTTI::setSubMesh, &MeshBaseRTTI::setNumSubmeshes);

			addPlainField("mDefaultSubMesh", 3, &MeshBaseRTTI::getDefaultSubMesh, &MeshBaseRTTI::setDefaultSubMesh);
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}

		virtual const String& getRTTIName() 
		{
			static String name = "MeshBase";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_MeshBase;
		}
	};
}