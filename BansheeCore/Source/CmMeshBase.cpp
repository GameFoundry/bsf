#include "CmMeshBase.h"
#include "CmMeshBaseRTTI.h"
#include "CmCoreThread.h"
#include "CmDebug.h"

namespace BansheeEngine
{
	MeshBase::MeshBase(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp)
		:mNumIndices(numIndices), mNumVertices(numVertices), mDefaultSubMesh(0, numIndices, drawOp)
	{
		mSubMeshes.reserve(10);
	}

	MeshBase::MeshBase()
	{
		mSubMeshes.reserve(10);
	}

	MeshBase::~MeshBase()
	{

	}

	void MeshBase::clearSubMeshes()
	{
		THROW_IF_CORE_THREAD;

		mSubMeshes.clear();
	}

	void MeshBase::addSubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp)
	{
		THROW_IF_CORE_THREAD;

		if((indexOffset + indexCount) > mNumIndices)
		{
			LOGWRN("Provided sub-mesh references indexes out of range. Sub-mesh range: " 
				+ toString(indexOffset) + " .. " + toString(indexOffset + indexCount) + "." \
				"Valid range is: 0 .. " + toString(mNumIndices) + ". Ignoring command.");

			return;
		}

		mSubMeshes.push_back(SubMesh(indexOffset, indexCount, drawOp));
	}

	void MeshBase::setSubMeshes(const Vector<SubMesh>& subMeshes)
	{
		THROW_IF_CORE_THREAD;

		for(auto& subMesh : subMeshes)
		{
			if((subMesh.indexOffset + subMesh.indexCount) > mNumIndices)
			{
				LOGWRN("Provided sub-mesh references indexes out of range. Sub-mesh range: " 
					+ toString(subMesh.indexOffset) + " .. " + toString(subMesh.indexOffset + subMesh.indexCount) + "." \
					"Valid range is: 0 .. " + toString(mNumIndices) + ". Ignoring command.");

				return;
			}
		}

		mSubMeshes = subMeshes;
	}

	const SubMesh& MeshBase::getSubMesh(UINT32 subMeshIdx) const
	{
		THROW_IF_CORE_THREAD;

		if(mSubMeshes.size() == 0 && subMeshIdx == 0)
		{
			return mDefaultSubMesh;
		}

		if(subMeshIdx < 0 || subMeshIdx >= mSubMeshes.size())
		{
			BS_EXCEPT(InvalidParametersException, "Invalid sub-mesh index (" 
				+ toString(subMeshIdx) + "). Number of sub-meshes available: " + toString((int)mSubMeshes.size()));
		}

		return mSubMeshes[subMeshIdx];
	}

	UINT32 MeshBase::getNumSubMeshes() const
	{
		THROW_IF_CORE_THREAD;

		if(mSubMeshes.size() > 0)
			return (UINT32)mSubMeshes.size();
		else
		{
			if(mDefaultSubMesh.indexCount > 0)
				return 1;
			else
				return 0;
		}
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