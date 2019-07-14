//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Components/BsCCollider.h"
#include "Scene/BsSceneObject.h"
#include "Components/BsCRigidbody.h"
#include "Physics/BsPhysics.h"
#include "Private/RTTI/BsCColliderRTTI.h"

using namespace std::placeholders;

namespace bs
{
	CCollider::CCollider()
	{
		setName("Collider");

		mNotifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
	}

	CCollider::CCollider(const HSceneObject& parent)
		: Component(parent)
	{
		setName("Collider");

		mNotifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
	}

	void CCollider::setIsTrigger(bool value)
	{
		if (mIsTrigger == value)
			return;

		mIsTrigger = value;

		if (mInternal != nullptr)
		{
			mInternal->setIsTrigger(value);

			updateParentRigidbody();
			updateTransform();
		}
	}

	void CCollider::setMass(float mass)
	{
		if (mMass == mass)
			return;

		mMass = mass;

		if (mInternal != nullptr)
		{
			mInternal->setMass(mass);

			if (mParent != nullptr)
				mParent->_updateMassDistribution();
		}
	}

	void CCollider::setMaterial(const HPhysicsMaterial& material)
	{
		mMaterial = material;

		if (mInternal != nullptr)
			mInternal->setMaterial(material);
	}

	void CCollider::setContactOffset(float value)
	{
		value = std::max(0.0f, std::max(value, getRestOffset()));

		mContactOffset = value;

		if (mInternal != nullptr)
			mInternal->setContactOffset(value);
	}

	void CCollider::setRestOffset(float value)
	{
		value = std::min(value, getContactOffset());

		mRestOffset = value;

		if (mInternal != nullptr)
			mInternal->setRestOffset(value);
	}

	void CCollider::setLayer(UINT64 layer)
	{
		mLayer = layer;

		if (mInternal != nullptr)
			mInternal->setLayer(layer);
	}

	void CCollider::setCollisionReportMode(CollisionReportMode mode)
	{
		mCollisionReportMode = mode;

		if (mInternal != nullptr)
			updateCollisionReportMode();
	}

	void CCollider::onInitialized()
	{

	}

	void CCollider::onDestroyed()
	{
		destroyInternal();
	}

	void CCollider::onDisabled()
	{
		destroyInternal();
	}

	void CCollider::onEnabled()
	{
		restoreInternal();
	}

	void CCollider::onTransformChanged(TransformChangedFlags flags)
	{
		if (!SO()->getActive())
			return;

		if ((flags & TCF_Parent) != 0)
			updateParentRigidbody();

		// Don't update the transform if it's due to Physics update since then we can guarantee it will remain at the same
		// relative transform to its parent
		if (gPhysics()._isUpdateInProgress())
			return;

		if ((flags & (TCF_Parent | TCF_Transform)) != 0)
			updateTransform();
	}

	void CCollider::setRigidbody(const HRigidbody& rigidbody, bool internal)
	{
		if (rigidbody == mParent)
			return;

		if (mInternal != nullptr && !internal)
		{
			if (mParent != nullptr)
				mParent->removeCollider(static_object_cast<CCollider>(mThisHandle));

			Rigidbody* rigidBodyPtr = nullptr;

			if (rigidbody != nullptr)
				rigidBodyPtr = rigidbody->_getInternal();

			mInternal->setRigidbody(rigidBodyPtr);

			if (rigidbody != nullptr)
				rigidbody->addCollider(static_object_cast<CCollider>(mThisHandle));
		}

		mParent = rigidbody;
		updateCollisionReportMode();
		updateTransform();
	}

	bool CCollider::rayCast(const Ray& ray, PhysicsQueryHit& hit, float maxDist) const
	{
		if (mInternal == nullptr)
			return false;

		return mInternal->rayCast(ray, hit, maxDist);
	}

	bool CCollider::rayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
		float maxDist) const
	{
		if (mInternal == nullptr)
			return false;

		return mInternal->rayCast(origin, unitDir, hit, maxDist);
	}

	void CCollider::restoreInternal()
	{
		if (mInternal == nullptr)
		{
			mInternal = createInternal();

			mInternal->onCollisionBegin.connect(std::bind(&CCollider::triggerOnCollisionBegin, this, _1));
			mInternal->onCollisionStay.connect(std::bind(&CCollider::triggerOnCollisionStay, this, _1));
			mInternal->onCollisionEnd.connect(std::bind(&CCollider::triggerOnCollisionEnd, this, _1));
		}

		// Note: Merge into one call to avoid many virtual function calls
		mInternal->setIsTrigger(mIsTrigger);
		mInternal->setMass(mMass);
		mInternal->setMaterial(mMaterial);
		mInternal->setContactOffset(mContactOffset);
		mInternal->setRestOffset(mRestOffset);
		mInternal->setLayer(mLayer);

		updateParentRigidbody();
		updateTransform();
		updateCollisionReportMode();
	}

	void CCollider::destroyInternal()
	{
		if (mParent != nullptr)
			mParent->removeCollider(static_object_cast<CCollider>(mThisHandle));

		mParent = nullptr;

		// This should release the last reference and destroy the internal collider
		if(mInternal)
		{
			mInternal->_setOwner(PhysicsOwnerType::None, nullptr);
			mInternal = nullptr;
		}
	}

	void CCollider::updateParentRigidbody()
	{
		if (mIsTrigger)
		{
			setRigidbody(HRigidbody());
			return;
		}

		HSceneObject currentSO = SO();
		while (currentSO != nullptr)
		{
			HRigidbody parent = currentSO->getComponent<CRigidbody>();
			if (parent != nullptr)
			{
				if(currentSO->getActive() && isValidParent(parent))
					setRigidbody(parent);
				else
					setRigidbody(HRigidbody());

				return;
			}

			currentSO = currentSO->getParent();
		}

		// Not found
		setRigidbody(HRigidbody());
	}

	void CCollider::updateTransform()
	{
		const Transform& tfrm = SO()->getTransform();
		Vector3 myScale = tfrm.getScale();

		if (mParent != nullptr)
		{
			const Transform& parentTfrm = mParent->SO()->getTransform();
			Vector3 parentPos = parentTfrm.getPosition();
			Quaternion parentRot = parentTfrm.getRotation();

			Vector3 myPos = tfrm.getPosition();
			Quaternion myRot = tfrm.getRotation();

			Vector3 scale = parentTfrm.getScale();
			Vector3 invScale = scale;
			if (invScale.x != 0) invScale.x = 1.0f / invScale.x;
			if (invScale.y != 0) invScale.y = 1.0f / invScale.y;
			if (invScale.z != 0) invScale.z = 1.0f / invScale.z;

			Quaternion invRotation = parentRot.inverse();

			Vector3 relativePos = invRotation.rotate(myPos - parentPos) *  invScale;
			Quaternion relativeRot = invRotation * myRot;

			relativePos = relativePos + relativeRot.rotate(mLocalPosition * scale);
			relativeRot = relativeRot * mLocalRotation;

			if(mInternal)
				mInternal->setTransform(relativePos, relativeRot);

			mParent->_updateMassDistribution();
		}
		else
		{
			Quaternion myRot = tfrm.getRotation();
			Vector3 myPos = tfrm.getPosition() + myRot.rotate(mLocalPosition * myScale);
			myRot = myRot * mLocalRotation;

			if(mInternal)
				mInternal->setTransform(myPos, myRot);
		}

		if (mInternal)
			mInternal->setScale(myScale);
	}

	void CCollider::updateCollisionReportMode()
	{
		CollisionReportMode mode = mCollisionReportMode;

		if (mParent != nullptr)
			mode = mParent->getCollisionReportMode();

		if(mInternal != nullptr)
			mInternal->setCollisionReportMode(mode);
	}

	void CCollider::triggerOnCollisionBegin(const CollisionDataRaw& data)
	{
		CollisionData hit;
		hit.contactPoints = data.contactPoints;
		hit.collider[0] = static_object_cast<CCollider>(mThisHandle);

		if(data.colliders[1] != nullptr)
		{
			CCollider* other = (CCollider*)data.colliders[1]->_getOwner(PhysicsOwnerType::Component);
			hit.collider[1] = static_object_cast<CCollider>(other->getHandle());
		}

		onCollisionBegin(hit);
	}

	void CCollider::triggerOnCollisionStay(const CollisionDataRaw& data)
	{
		CollisionData hit;
		hit.contactPoints = data.contactPoints;
		hit.collider[0] = static_object_cast<CCollider>(mThisHandle);

		if (data.colliders[1] != nullptr)
		{
			CCollider* other = (CCollider*)data.colliders[1]->_getOwner(PhysicsOwnerType::Component);
			hit.collider[1] = static_object_cast<CCollider>(other->getHandle());
		}

		onCollisionStay(hit);
	}

	void CCollider::triggerOnCollisionEnd(const CollisionDataRaw& data)
	{
		CollisionData hit;
		hit.contactPoints = data.contactPoints;
		hit.collider[0] = static_object_cast<CCollider>(mThisHandle);

		if (data.colliders[1] != nullptr)
		{
			CCollider* other = (CCollider*)data.colliders[1]->_getOwner(PhysicsOwnerType::Component);
			hit.collider[1] = static_object_cast<CCollider>(other->getHandle());
		}

		onCollisionEnd(hit);
	}

	RTTITypeBase* CCollider::getRTTIStatic()
	{
		return CColliderRTTI::instance();
	}

	RTTITypeBase* CCollider::getRTTI() const
	{
		return CCollider::getRTTIStatic();
	}
}
