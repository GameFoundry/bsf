//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCCollider.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsPhysics.h"
#include "BsCColliderRTTI.h"

using namespace std::placeholders;

namespace BansheeEngine
{
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
		mContactOffset = value;

		if (mInternal != nullptr)
			mInternal->setContactOffset(value);
	}

	void CCollider::setRestOffset(float value)
	{
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

	void CCollider::onInitialized()
	{

	}

	void CCollider::onDestroyed()
	{
		// This should release the last reference and destroy the internal collider
		mInternal = nullptr;
	}

	void CCollider::onDisabled()
	{
		mParent = nullptr;

		// This should release the last reference and destroy the internal collider
		mInternal = nullptr;
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

	void CCollider::_setRigidbody(const HRigidbody& rigidbody)
	{
		SPtr<Rigidbody> rigidBodyPtr;

		if (rigidbody != nullptr)
			rigidBodyPtr = rigidbody->_getInternal();

		if (mInternal == nullptr)
		{
			mInternal->setRigidbody(rigidBodyPtr);
			mParent = rigidbody;
		}
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

		if (mParent != nullptr)
			mInternal->setRigidbody(mParent->_getInternal());

		updateTransform();
	}

	void CCollider::updateParentRigidbody()
	{
		mParent = nullptr;

		if (mIsTrigger)
			return;

		HSceneObject currentSO = SO();
		while (currentSO != nullptr)
		{
			mParent = currentSO->getComponent<CRigidbody>();
			if (mParent != nullptr)
				return;

			currentSO = currentSO->getParent();
		}
	}

	void CCollider::updateTransform()
	{
		if (mParent != nullptr)
		{
			Vector3 parentPos = mParent->SO()->getWorldPosition();
			Quaternion parentRot = mParent->SO()->getWorldRotation();

			Vector3 myPos = SO()->getWorldPosition();
			Quaternion myRot = SO()->getWorldRotation();

			Vector3 scale = mParent->SO()->getWorldScale();
			Vector3 invScale = scale;
			if (invScale.x != 0) invScale.x = 1.0f / invScale.x;
			if (invScale.y != 0) invScale.y = 1.0f / invScale.y;
			if (invScale.z != 0) invScale.z = 1.0f / invScale.z;

			Quaternion invRotation = parentRot.inverse();

			Vector3 relativePos = invRotation.rotate(myPos - parentPos) *  invScale;
			Quaternion relativeRot = invRotation * myRot;

			relativePos = relativePos + myRot.rotate(mLocalPosition * scale);
			relativeRot = relativeRot * mLocalRotation;

			mInternal->setTransform(relativePos, relativeRot);
			mParent->_updateMassDistribution();
		}
		else
		{
			Vector3 myScale = SO()->getWorldScale();
			Quaternion myRot = SO()->getWorldRotation();
			Vector3 myPos = SO()->getWorldPosition() + myRot.rotate(mLocalPosition * myScale);
			myRot = myRot * mLocalRotation;

			mInternal->setTransform(myPos, myRot);
		}
	}

	void CCollider::triggerOnCollisionBegin(const CollisionData& data)
	{
		onCollisionBegin(data);
	}

	void CCollider::triggerOnCollisionStay(const CollisionData& data)
	{
		onCollisionStay(data);
	}

	void CCollider::triggerOnCollisionEnd(const CollisionData& data)
	{
		onCollisionEnd(data);
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