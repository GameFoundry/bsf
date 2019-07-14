//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "Physics/BsHingeJoint.h"
#include "PxPhysics.h"
#include "extensions/PxRevoluteJoint.h"

namespace bs
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of a HingeJoint. */
	class PhysXHingeJoint : public HingeJoint
	{
	public:
		PhysXHingeJoint(physx::PxPhysics* physx, const HINGE_JOINT_DESC& desc);
		~PhysXHingeJoint();

		/** @copydoc HingeJoint::getAngle */
		Radian getAngle() const override;

		/** @copydoc HingeJoint::getSpeed */
		float getSpeed() const override;

		/** @copydoc HingeJoint::getLimit */
		LimitAngularRange getLimit() const override;

		/** @copydoc HingeJoint::setLimit */
		void setLimit(const LimitAngularRange& limit) override;

		/** @copydoc HingeJoint::getDrive */
		HingeJointDrive getDrive() const override;

		/** @copydoc HingeJoint::setDrive */
		void setDrive(const HingeJointDrive& drive) override;

		/** @copydoc HingeJoint::setFlag */
		void setFlag(HingeJointFlag flag, bool enabled) override;

		/** @copydoc HingeJoint::hasFlag */
		bool hasFlag(HingeJointFlag flag) const override;

	private:
		/** Returns the internal PhysX representation of the hinge (revolute) joint. */
		inline physx::PxRevoluteJoint* getInternal() const;
	};

	/** @} */
}
