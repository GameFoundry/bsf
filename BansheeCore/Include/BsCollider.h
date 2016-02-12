//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollision.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Collider
	{
	public:
		virtual ~Collider() { }

		inline Vector3 getPosition() const;
		inline Quaternion getRotation() const;
		inline void setTransform(const Vector3& pos, const Quaternion& rot);

		virtual void setScale(const Vector3& scale);
		inline Vector3 getScale() const;

		inline void setIsTrigger(bool value);
		inline bool getIsTrigger() const;

		inline void setRigidbody(const SPtr<Rigidbody>& value);
		SPtr<Rigidbody> getRigidbody() const { return mRigidbody; }

		inline void setMass(float mass);
		inline float getMass() const;

		inline void setMaterial(const HPhysicsMaterial& material);
		inline HPhysicsMaterial getMaterial() const;

		inline void setContactOffset(float value);
		inline float getContactOffset();

		inline void setRestOffset(float value);
		inline float getRestOffset();

		inline void setLayer(UINT64 layer);
		inline UINT64 getLayer() const;

		Event<void(const CollisionData&)> onCollisionBegin;
		Event<void(const CollisionData&)> onCollisionStay;
		Event<void(const CollisionData&)> onCollisionEnd;

		FCollider* _getInternal() const { return mInternal; }
	protected:
		FCollider* mInternal = nullptr;
		SPtr<Rigidbody> mRigidbody;
		Vector3 mScale = Vector3::ONE;
	};
}