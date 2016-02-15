//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsD6Joint.h"
#include "PxPhysics.h"
#include "extensions\PxD6Joint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXD6Joint : public D6Joint
	{
	public:
		PhysXD6Joint(physx::PxPhysics* physx);
		~PhysXD6Joint();

		Motion getMotion(Axis axis) const override;
		void setMotion(Axis axis, Motion motion) override;

		Radian getTwist() const override;
		Radian getSwingY() const override;
		Radian getSwingZ() const override;

		LimitLinear getLimitLinear() const override;
		void setLimitLinear(const LimitLinear& limit) override;

		LimitAngularRange getLimitTwist() const override;
		void setLimitTwist(const LimitAngularRange& limit) override;

		LimitConeRange getLimitSwing() const override;
		void setLimitSwing(const LimitConeRange& limit) override;

		Drive getDrive(DriveType type) const override;
		void setDrive(DriveType type, const Drive& drive) override;

		Vector3 getDrivePosition() const override;
		Quaternion getDriveRotation() const override;
		void setDriveTransform(const Vector3& position, const Quaternion& rotation) override;

		Vector3 getDriveLinearVelocity() const override;
		Vector3 getDriveAngularVelocity() const override;
		void setDriveVelocity(const Vector3& linear, const Vector3& angular) override;

	private:
		inline physx::PxD6Joint* getInternal() const;
	};
}