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
		:mShape(shape)
	{
		mStaticBody = gPhysX().getPhysX()->createRigidStatic(PxTransform(PxIdentity));
		mStaticBody->attachShape(*mShape);

		PxScene* scene = gPhysX().getScene();
		scene->addActor(*mStaticBody);

		updateFilter();
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

	void FPhysXCollider::setIsStatic(bool value)
	{
		if (mIsStatic == value)
			return;

		if (mStaticBody != nullptr)
		{
			mStaticBody->detachShape(*mShape);

			mStaticBody->release();
			mStaticBody = nullptr;
		}

		mIsStatic = value;

		if (mIsStatic)
		{
			mStaticBody = gPhysX().getPhysX()->createRigidStatic(PxTransform(PxIdentity));
			mStaticBody->attachShape(*mShape);

			PxScene* scene = gPhysX().getScene();
			scene->addActor(*mStaticBody);
		}
	}

	bool FPhysXCollider::getIsStatic() const
	{
		return mIsStatic;
	}

	void FPhysXCollider::setContactOffset(float value)
	{
		mShape->setContactOffset(value);
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
			materials[0] = gPhysX().getDefaultMaterial();

		mShape->setMaterials(materials, sizeof(materials) / sizeof(materials[0]));
	}

	UINT64 FPhysXCollider::getLayer() const
	{
		return mLayer;
	}

	void FPhysXCollider::setLayer(UINT64 layer)
	{
		mLayer = layer;
		updateFilter();
	}

	CollisionReportMode FPhysXCollider::getCollisionReportMode() const
	{
		return mCollisionReportMode;
	}

	void FPhysXCollider::setCollisionReportMode(CollisionReportMode mode)
	{
		mCollisionReportMode = mode;
		updateFilter();
	}

	void FPhysXCollider::_setCCD(bool enabled)
	{
		mCCD = enabled;
		updateFilter();
	}

	void FPhysXCollider::updateFilter()
	{
		PxFilterData data;
		memcpy(&data.word0, &mLayer, sizeof(mLayer));
		mShape->setSimulationFilterData(data);
		mShape->setQueryFilterData(data);

		PhysXObjectFilterFlags flags;

		switch(mCollisionReportMode)
		{
		case CollisionReportMode::None:
			flags |= PhysXObjectFilterFlag::NoReport;
		case CollisionReportMode::Report:
			flags |= PhysXObjectFilterFlag::ReportBasic;
		case CollisionReportMode::ReportPersistent:
			flags |= PhysXObjectFilterFlag::ReportAll;
		}

		if (mCCD)
			flags |= PhysXObjectFilterFlag::CCD;

		data.word2 = flags;
	}
}