#include "BsPhysXMeshCollider.h"
#include "BsPhysX.h"
#include "PxPhysics.h"
#include "BsFPhysXCollider.h"
#include "BsPhysXMesh.h"

using namespace physx;

namespace BansheeEngine
{
	PhysXMeshCollider::PhysXMeshCollider(PxPhysics* physx, const Vector3& position, const Quaternion& rotation)
	{
		PxConvexMeshGeometry geometry;

		PxShape* shape = physx->createShape(geometry, *gPhysX().getDefaultMaterial(), true);
		shape->setLocalPose(toPxTransform(position, rotation));
		shape->userData = this;

		mInternal = bs_new<FPhysXCollider>(shape);
	}

	PhysXMeshCollider::~PhysXMeshCollider()
	{
		bs_delete(mInternal);
	}

	void PhysXMeshCollider::setScale(const Vector3& scale)
	{
		MeshCollider::setScale(scale);
		applyGeometry();
	}

	void PhysXMeshCollider::onMeshChanged()
	{
		applyGeometry();
	}

	void PhysXMeshCollider::applyGeometry()
	{
		if (!mMesh.isLoaded())
		{
			PxConvexMeshGeometry geometry;
			getInternal()->_getShape()->setGeometry(geometry);
			return;
		}

		SPtr<PhysXMesh> physxMesh = std::static_pointer_cast<PhysXMesh>(mMesh.getInternalPtr());

		if (physxMesh->getType() == PhysicsMeshType::Convex)
		{
			PxConvexMeshGeometry geometry;
			geometry.scale = PxMeshScale(toPxVector(getScale()), PxIdentity);
			geometry.convexMesh = physxMesh->_getConvex();

			getInternal()->_getShape()->setGeometry(geometry);
		}
		else // Triangle
		{
			PxTriangleMeshGeometry geometry;
			geometry.scale = PxMeshScale(toPxVector(getScale()), PxIdentity);
			geometry.triangleMesh = physxMesh->_getTriangle();

			getInternal()->_getShape()->setGeometry(geometry);
		}
	}

	FPhysXCollider* PhysXMeshCollider::getInternal() const
	{
		return static_cast<FPhysXCollider*>(mInternal);
	}
}