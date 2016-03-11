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
		PxSphereGeometry geometry(0.01f); // Dummy

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
			PxSphereGeometry geometry(0.01f); // Dummy
			getInternal()->_getShape()->setGeometry(geometry);
			return;
		}

		FPhysXMesh* physxMesh = static_cast<FPhysXMesh*>(mMesh->_getInternal());

		if (mMesh->getType() == PhysicsMeshType::Convex)
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