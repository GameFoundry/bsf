#include "BsCollider.h"
#include "BsFCollider.h"

namespace BansheeEngine
{
	Vector3 Collider::getPosition() const
	{
		return mInternal->getPosition();
	}

	Quaternion Collider::getRotation() const
	{
		return mInternal->getRotation();
	}

	void Collider::setTransform(const Vector3& pos, const Quaternion& rot)
	{
		mInternal->setTransform(pos, rot);
	}

	void Collider::setIsTrigger(bool value)
	{
		mInternal->setIsTrigger(value);
	}

	void Collider::setMass(float mass)
	{
		mInternal->setMass(mass);
	}

	float Collider::getMass() const
	{
		return mInternal->getMass();
	}

	bool Collider::getIsTrigger() const
	{
		return mInternal->getIsTrigger();
	}

	void Collider::setRigidbody(const SPtr<Rigidbody>& rigidbody)
	{
		mInternal->setRigidbody(rigidbody);
	}

	SPtr<Rigidbody> Collider::getRigidbody() const
	{
		return mInternal->getRigidbody();
	}

	void Collider::setMaterial(const HPhysicsMaterial& material)
	{
		mInternal->setMaterial(material);
	}

	HPhysicsMaterial Collider::getMaterial() const
	{
		return mInternal->getMaterial();
	}

	void Collider::setContactOffset(float value)
	{
		mInternal->setContactOffset(value);
	}

	float Collider::getContactOffset()
	{
		return mInternal->getContactOffset();
	}

	void Collider::setRestOffset(float value)
	{
		mInternal->setRestOffset(value);
	}

	float Collider::getRestOffset()
	{
		return mInternal->getRestOffset();
	}

	void Collider::setLayer(UINT64 layer)
	{
		mInternal->setLayer(layer);
	}

	UINT64 Collider::getLayer() const
	{
		return mInternal->getLayer();
	}
}