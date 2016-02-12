//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollider.h"
#include "BsIResourceListener.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT MeshCollider : public Collider, public IResourceListener
	{
	public:
		MeshCollider();

		void setMesh(const HPhysicsMesh& mesh) { mMesh = mesh; onMeshChanged();  markListenerResourcesDirty(); }
		HPhysicsMesh getMesh() const { return mMesh; }

		static SPtr<MeshCollider> create(const Vector3& position = Vector3::ZERO, 
			const Quaternion& rotation = Quaternion::IDENTITY);

	protected:
		/** @copydoc IResourceListener::getListenerResources */
		void getListenerResources(Vector<HResource>& resources) override;

		/** @copydoc IResourceListener::notifyResourceLoaded */
		void notifyResourceLoaded(const HResource& resource) override;

		/** @copydoc IResourceListener::notifyResourceChanged */
		void notifyResourceChanged(const HResource& resource) override;

		virtual void onMeshChanged() { }

		HPhysicsMesh mMesh;
	};
}