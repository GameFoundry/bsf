//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsJoint.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT D6Joint : public Joint
	{
	public:
		enum class Axis
		{
			X, Y, Z, Twist, SwingY, SwingZ
		};

		enum class Motion
		{
			Locked, Limited, Free
		};

		enum class DriveType
		{
			X, Y, Z, Swing, Twist, SLERP
		};

		struct Drive
		{
			float stiffness = 0.0f;
			float damping = 0.0f;
			float forceLimit = FLT_MAX;
			bool acceleration = false;
		};

	public:
		virtual ~D6Joint() { }

		virtual Motion getMotion(Axis axis) const = 0;
		virtual void setMotion(Axis axis, Motion motion) = 0;

		virtual Radian getTwist() const = 0;
		virtual Radian getSwingY() const = 0;
		virtual Radian getSwingZ() const = 0;

		virtual LimitLinear getLimitLinear() const = 0;
		virtual void setLimitLinear(const LimitLinear& limit) = 0;
		
		virtual LimitAngularRange getLimitTwist() const = 0;
		virtual void setLimitTwist(const LimitAngularRange& limit) = 0;

		virtual LimitConeRange getLimitSwing() const = 0;
		virtual void setLimitSwing(const LimitConeRange& limit) = 0;
	
		virtual Drive getDrive(DriveType type) const = 0;
		virtual void setDrive(DriveType type, const Drive& drive) = 0;

		virtual Vector3 getDrivePosition() const = 0;
		virtual Quaternion getDriveRotation() const = 0;
		virtual void setDriveTransform(const Vector3& position, const Quaternion& rotation) = 0;

		virtual Vector3 getDriveLinearVelocity() const = 0;
		virtual Vector3 getDriveAngularVelocity() const = 0;
		virtual void setDriveVelocity(const Vector3& linear, const Vector3& angular) = 0;

		static SPtr<D6Joint> create();
	};
}