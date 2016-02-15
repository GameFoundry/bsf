//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsFJoint.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT Joint
	{
	public:
		virtual ~Joint() { }

		inline Rigidbody* getBody(JointBody body) const;
		inline void setBody(JointBody body, Rigidbody* value);

		inline Vector3 getPosition(JointBody body) const;
		inline Quaternion getRotation(JointBody body) const;
		inline void setTransform(JointBody body, const Vector3& position, const Quaternion& rotation);

		inline float getBreakForce() const;
		inline void setBreakForce(float force);

		inline float getBreakTorque() const;
		inline void setBreakToque(float torque);

	protected:
		FJoint* mInternal = nullptr;
	};

	struct Spring
	{
		Spring() { }
		Spring(float stiffness, float damping)
			:stiffness(stiffness), damping(damping)
		{ }

		float stiffness = 0.0f;
		float damping = 0.0f;
	};

	struct LimitLinear
	{
		LimitLinear()
		{ }

		LimitLinear(float extent, float contactDist = -1.0f)
			:extent(extent), contactDist(contactDist)
		{ }

		LimitLinear(float extent, const Spring& spring)
			:extent(extent), spring(spring)
		{ }

		float extent = FLT_MAX;
		float contactDist = -1.0f;
		Spring spring;
	};

	struct LimitAngular
	{
		LimitAngular()
		{ }

		LimitAngular(Radian lower, Radian upper, float contactDist = -1.0f)
			:lower(lower), upper(upper), contactDist(contactDist)
		{ }

		LimitAngular(Radian lower, Radian upper, const Spring& spring)
			:lower(lower), upper(upper), spring(spring)
		{ }

		Radian lower = Radian(0.0f);
		Radian upper = Radian(0.0f);
		float contactDist = -1.0f;
		Spring spring;
	};

	struct LimitCone
	{
		LimitCone()
		{ }

		LimitCone(Radian yLimitAngle, Radian zLimitAngle, float contactDist = -1.0f)
			:yLimitAngle(yLimitAngle), zLimitAngle(zLimitAngle), contactDist(contactDist)
		{ }

		LimitCone(Radian yLimitAngle, Radian zLimitAngle, const Spring& spring)
			:yLimitAngle(yLimitAngle), zLimitAngle(zLimitAngle), spring(spring)
		{ }

		Radian yLimitAngle = Radian(Math::PI * 0.5f);
		Radian zLimitAngle = Radian(Math::PI * 0.5f);
		float contactDist = -1.0f;
		Spring spring;
	};
}