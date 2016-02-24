//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsPhysicsCommon.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT FCollider
	{
	public:
		virtual ~FCollider();

		virtual Vector3 getPosition() const = 0;
		virtual Quaternion getRotation() const = 0;
		virtual void setTransform(const Vector3& pos, const Quaternion& rotation) = 0;

		virtual void setIsTrigger(bool value) = 0;
		virtual bool getIsTrigger() const = 0;

		virtual void setIsStatic(bool value) = 0;
		virtual bool getIsStatic() const = 0;

		// Not used for triggers, only relevant if parent rigidbody uses child mass
		virtual void setMass(float mass) { mMass = mass; }
		virtual float getMass() const { return mMass; }

		virtual void setMaterial(const HPhysicsMaterial& material);
		virtual HPhysicsMaterial getMaterial() const { return mMaterial; }

		/**
		 * Determines how far apart do two shapes need to be away from each other before the physics runtime starts 
		 * generating repelling impulse for them. This distance will be the sum of contact offsets of the two interacting
		 * objects. If objects are moving fast you can increase this value to start generating the impulse earlier and 
		 * potentially prevent the objects from interpenetrating. This value is in meters.
		 *
		 * Also see setRestOffset().
		 */
		virtual void setContactOffset(float value) = 0;

		/** Returns shape's contact offset in meters. See setContactOffset() to learn contact offset is. */
		virtual float getContactOffset() const = 0;

		/**
		 * Sets at what distance should two objects resting on one another come to an equilibrium. The value used in the
		 * runtime will be the sum of rest offsets for both interacting objects. This value is in meters.
		 */
		virtual void setRestOffset(float value) = 0;

		/** Returns shepe's rest offset in meters. See setRestOffset() to learn what contact offset is. */
		virtual float getRestOffset() const = 0;

		virtual void setLayer(UINT64 layer) = 0;
		virtual UINT64 getLayer() const = 0;

		virtual void setCollisionReportMode(CollisionReportMode mode);
		virtual CollisionReportMode getCollisionReportMode() const = 0;

		/** Enables continous collision detect for this collider. Only valid if the collider is a part of a rigidbody. */
		virtual void _setCCD(bool enabled) = 0;
	protected:
		float mMass = 1.0f;
		
		HPhysicsMaterial mMaterial;
	};
}