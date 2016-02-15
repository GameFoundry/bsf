//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	enum class JointBody
	{
		A, B
	};

	class BS_CORE_EXPORT FJoint
	{
	public:
		virtual ~FJoint() { }

		virtual Rigidbody* getBody(JointBody body) const = 0;
		virtual void setBody(JointBody body, Rigidbody* value) = 0;

		virtual Vector3 getPosition(JointBody body) const = 0;
		virtual Quaternion getRotation(JointBody body) const = 0;
		virtual void setTransform(JointBody body, const Vector3& position, const Quaternion& rotation) = 0;

		virtual float getBreakForce() const = 0;
		virtual void setBreakForce(float force) = 0;

		virtual float getBreakTorque() const = 0;
		virtual void setBreakToque(float torque) = 0;
	};
}