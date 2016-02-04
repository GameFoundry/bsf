#include "BsFPhysXCollider.h"
#include "BsPhysX.h"
#include "BsPhysXRigidbody.h"
#include "BsPhysXMaterial.h"
#include "PxScene.h"
#include "PxShape.h"

using namespace physx;

namespace BansheeEngine
{
	FPhysXCollider::FPhysXCollider(PxShape* shape)
		:mShape(shape), mStaticBody(nullptr), mIsTrigger(false)
	{
		mStaticBody = gPhysX().getPhysX()->createRigidStatic(PxTransform());
		mStaticBody->attachShape(*mShape);

		PxScene* scene = gPhysX().getScene();
		scene->addActor(*mStaticBody);
	}

	FPhysXCollider::~FPhysXCollider()
	{
		if (mStaticBody != nullptr)
			mStaticBody->release();

		mShape->release();
	}

	Vector3 FPhysXCollider::getPosition() const
	{
		return fromPxVector(mShape->getLocalPose().p);
	}

	Quaternion FPhysXCollider::getRotation() const
	{
		return fromPxQuaternion(mShape->getLocalPose().q);
	}

	void FPhysXCollider::setTransform(const Vector3& pos, const Quaternion& rotation)
	{
		mShape->setLocalPose(toPxTransform(pos, rotation));
	}

	void FPhysXCollider::setIsTrigger(bool value)
	{
		if(value)
		{
			mShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			mShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

			mIsTrigger = true;
		}
		else
		{
			mShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
			mShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

			mIsTrigger = false;
		}		
	}

	bool FPhysXCollider::getIsTrigger() const
	{
		return (UINT32)(mShape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE) != 0;
	}

	void FPhysXCollider::setContactOffset(float value)
	{
		mShape->setContactOffset(value);
	}

	void FPhysXCollider::setMass(float mass)
	{
		FCollider::setMass(mass);

		if (mRigidbody != nullptr)
			mRigidbody->_updateMassDistribution(); // Note: Perhaps I can just mark mass distribution as dirty and recalculate it delayed, in case a lot of colliders change
	}

	float FPhysXCollider::getContactOffset() const
	{
		return mShape->getContactOffset();
	}

	void FPhysXCollider::setRestOffset(float value)
	{
		mShape->setRestOffset(value);
	}

	float FPhysXCollider::getRestOffset() const
	{
		return mShape->getRestOffset();
	}

	void FPhysXCollider::setRigidbody(const SPtr<Rigidbody>& rigidbody)
	{
		if (mRigidbody == rigidbody)
			return;

		if(mStaticBody != nullptr)
		{
			mStaticBody->detachShape(*mShape);

			mStaticBody->release();
			mStaticBody = nullptr;
		}

		FCollider::setRigidbody(rigidbody);

		if(rigidbody == nullptr)
		{
			mStaticBody = gPhysX().getPhysX()->createRigidStatic(PxTransform());
			mStaticBody->attachShape(*mShape);

			PxScene* scene = gPhysX().getScene();
			scene->addActor(*mStaticBody);
		}
	}

	void FPhysXCollider::setMaterial(const HPhysicsMaterial& material)
	{
		FCollider::setMaterial(material);

		PhysXMaterial* physXmaterial = nullptr;
		if(material.isLoaded())
			physXmaterial = static_cast<PhysXMaterial*>(material.get());

		PxMaterial* materials[1];
		if (physXmaterial != nullptr)
			materials[0] = physXmaterial->_getInternal();
		else
			materials[0] = nullptr;

		mShape->setMaterials(materials, sizeof(materials));
	}

	void FPhysXCollider::setIsActive(bool value)
	{
		// Note: A different option might be just to fully destroy the shape & actor when disabled (might result in better
		//       performance if a lot of disabled actors in scene).

		if (!value)
			mShape->setFlags((PxShapeFlags)0);
		else
		{
			mShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			setIsTrigger(mIsTrigger);
		}

		FCollider::setIsActive(value);
	}

	void FPhysXCollider::setLayer(UINT64 layer)
	{
		PxFilterData data;
		memcpy(&data.word0, &layer, sizeof(layer));
		mShape->setSimulationFilterData(data);
	}
}