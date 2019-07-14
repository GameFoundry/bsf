//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "Physics/BsD6Joint.h"
#include "PxPhysics.h"
#include "extensions/PxD6Joint.h"

namespace bs
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of a D6 joint. */
	class PhysXD6Joint : public D6Joint
	{
	public:
		PhysXD6Joint(physx::PxPhysics* physx, const D6_JOINT_DESC& desc);
		~PhysXD6Joint();

		/** @copydoc D6Joint::getMotion */
		D6JointMotion getMotion(D6JointAxis axis) const override;

		/** @copydoc D6Joint::setMotion */
		void setMotion(D6JointAxis axis, D6JointMotion motion) override;

		/** @copydoc D6Joint::getTwist */
		Radian getTwist() const override;

		/** @copydoc D6Joint::getSwingY */
		Radian getSwingY() const override;

		/** @copydoc D6Joint::getSwingZ */
		Radian getSwingZ() const override;

		/** @copydoc D6Joint::getLimitLinear */
		LimitLinear getLimitLinear() const override;

		/** @copydoc D6Joint::setLimitLinear */
		void setLimitLinear(const LimitLinear& limit) override;

		/** @copydoc D6Joint::getLimitTwist */
		LimitAngularRange getLimitTwist() const override;

		/** @copydoc D6Joint::setLimitTwist */
		void setLimitTwist(const LimitAngularRange& limit) override;

		/** @copydoc D6Joint::getLimitSwing */
		LimitConeRange getLimitSwing() const override;

		/** @copydoc D6Joint::setLimitSwing */
		void setLimitSwing(const LimitConeRange& limit) override;

		/** @copydoc D6Joint::getDrive */
		D6JointDrive getDrive(D6JointDriveType type) const override;

		/** @copydoc D6Joint::setDrive */
		void setDrive(D6JointDriveType type, const D6JointDrive& drive) override;

		/** @copydoc D6Joint::getDrivePosition */
		Vector3 getDrivePosition() const override;

		/** @copydoc D6Joint::getDriveRotation */
		Quaternion getDriveRotation() const override;

		/** @copydoc D6Joint::setDriveTransform */
		void setDriveTransform(const Vector3& position, const Quaternion& rotation) override;

		/** @copydoc D6Joint::getDriveLinearVelocity */
		Vector3 getDriveLinearVelocity() const override;

		/** @copydoc D6Joint::getDriveAngularVelocity */
		Vector3 getDriveAngularVelocity() const override;

		/** @copydoc D6Joint::setDriveVelocity */
		void setDriveVelocity(const Vector3& linear, const Vector3& angular) override;

	private:
		/** Returns the internal PhysX representation of the D6 joint. */
		inline physx::PxD6Joint* getInternal() const;
	};

	/** @} */
}
