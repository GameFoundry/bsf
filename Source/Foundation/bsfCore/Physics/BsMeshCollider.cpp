//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsMeshCollider.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<MeshCollider> MeshCollider::create(const Vector3& position, const Quaternion& rotation)
	{
		return Physics::instance().createMeshCollider(position, rotation);
	}

	void MeshCollider::getListenerResources(Vector<HResource>& resources)
	{
		if(mMesh != nullptr)
			resources.push_back(mMesh);
	}

	void MeshCollider::notifyResourceLoaded(const HResource& resource)
	{
		onMeshChanged();
	}

	void MeshCollider::notifyResourceChanged(const HResource& resource)
	{
		onMeshChanged();
	}
}