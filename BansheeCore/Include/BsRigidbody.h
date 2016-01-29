//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollision.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Rigidbody
	{
	public:
		virtual ~Rigidbody();

		// TODO

		virtual void addCollider(Collider* collider);
		virtual void removeCollider(Collider* collider);

		virtual void setIsActive(bool value);
		virtual bool getIsActive() const { return mIsActive; }

		static SPtr<Rigidbody> create(const Vector3& position, const Quaternion& rotation);

		Event<void(const CollisionData&)> onCollisionStart;
		Event<void(const CollisionData&)> onCollisionStay;
		Event<void(const CollisionData&)> onCollisionEnd;
	protected:
		bool mIsActive = true;

		Vector<Collider*> mColliders;
	};
}