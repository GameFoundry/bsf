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
		inline void setBreakTorque(float torque);

		inline bool getEnableCollision() const;
		inline void setEnableCollision(bool value);

		Event<void()> onJointBreak;
	protected:
		FJoint* mInternal = nullptr;
	};

	struct Spring
	{
		Spring() { }
		Spring(float stiffness, float damping)
			:stiffness(stiffness), damping(damping)
		{ }

		bool operator==(const Spring& other) const
		{
			return stiffness == other.stiffness && damping == other.damping;
		}

		float stiffness = 0.0f;
		float damping = 0.0f;
	};

	struct LimitLinearRange
	{
		LimitLinearRange()
		{ }

		LimitLinearRange(float lower, float upper, float contactDist = -1.0f)
			:lower(lower), upper(upper), contactDist(contactDist)
		{ }

		LimitLinearRange(float lower, float upper, const Spring& spring)
			:lower(lower), upper(upper), spring(spring)
		{ }

		bool operator==(const LimitLinearRange& other) const
		{
			return lower == other.lower && upper == other.upper && contactDist == other.contactDist && 
				restitution == other.restitution && spring == other.spring;
		}

		float lower = 0.0f;
		float upper = 0.0f;
		float contactDist = -1.0f;
		float restitution = 0.0f;
		Spring spring;
	};

	struct LimitLinear
	{
		LimitLinear()
		{ }

		LimitLinear(float extent, float contactDist = -1.0f)
			:extent(extent),  contactDist(contactDist)
		{ }

		LimitLinear(float extent,const Spring& spring)
			:extent(extent), spring(spring)
		{ }

		bool operator==(const LimitLinear& other) const
		{
			return extent == other.extent && contactDist == other.contactDist && restitution == other.restitution &&
				spring == other.spring;
		}

		float extent = 0.0f;
		float contactDist = -1.0f;
		float restitution = 0.0f;
		Spring spring;
	};

	struct LimitAngularRange
	{
		LimitAngularRange()
		{ }

		LimitAngularRange(Radian lower, Radian upper, float contactDist = -1.0f)
			:lower(lower), upper(upper), contactDist(contactDist)
		{ }

		LimitAngularRange(Radian lower, Radian upper, const Spring& spring)
			:lower(lower), upper(upper), spring(spring)
		{ }

		bool operator==(const LimitAngularRange& other) const
		{
			return lower == other.lower && upper == other.upper && contactDist == other.contactDist && 
				restitution == other.restitution && spring == other.spring;
		}

		Radian lower = Radian(0.0f);
		Radian upper = Radian(0.0f);
		float contactDist = -1.0f;
		float restitution = 0.0f;
		Spring spring;
	};

	struct LimitConeRange
	{
		LimitConeRange()
		{ }

		LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, float contactDist = -1.0f)
			:yLimitAngle(yLimitAngle), zLimitAngle(zLimitAngle), contactDist(contactDist)
		{ }

		LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, const Spring& spring)
			:yLimitAngle(yLimitAngle), zLimitAngle(zLimitAngle), spring(spring)
		{ }

		bool operator==(const LimitConeRange& other) const
		{
			return yLimitAngle == other.yLimitAngle && zLimitAngle == other.zLimitAngle && 
				contactDist == other.contactDist && restitution == other.restitution && spring == other.spring;
		}

		Radian yLimitAngle = Radian(Math::PI * 0.5f);
		Radian zLimitAngle = Radian(Math::PI * 0.5f);
		float contactDist = -1.0f;
		float restitution = 0.0f;
		Spring spring;
	};
}