//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsMeshBase.h"
#include "BsMeshBaseRTTI.h"
#include "BsCoreThread.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	MeshBase::MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
		:mNumIndices(numIndices), mNumVertices(numVertices), mCoreDirtyFlags(0xFFFFFF)
	{
		mSubMeshes.push_back(SubMesh(0, numIndices, drawOp));
		mActiveProxies.resize(mSubMeshes.size());
	}

	MeshBase::MeshBase(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes)
		: mNumIndices(numIndices), mNumVertices(numVertices), mCoreDirtyFlags(0xFFFFFF)
	{
		mSubMeshes = subMeshes;
		mActiveProxies.resize(mSubMeshes.size());
	}

	MeshBase::MeshBase()
	{
		mSubMeshes.reserve(10);
	}

	MeshBase::~MeshBase()
	{

	}

	const SubMesh& MeshBase::getSubMesh(UINT32 subMeshIdx) const
	{
		if(subMeshIdx < 0 || subMeshIdx >= mSubMeshes.size())
		{
			BS_EXCEPT(InvalidParametersException, "Invalid sub-mesh index (" 
				+ toString(subMeshIdx) + "). Number of sub-meshes available: " + toString((int)mSubMeshes.size()));
		}

		return mSubMeshes[subMeshIdx];
	}

	UINT32 MeshBase::getNumSubMeshes() const
	{
		return (UINT32)mSubMeshes.size();
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* MeshBase::getRTTIStatic()
	{
		return MeshBaseRTTI::instance();
	}

	RTTITypeBase* MeshBase::getRTTI() const
	{
		return MeshBase::getRTTIStatic();
	}
}