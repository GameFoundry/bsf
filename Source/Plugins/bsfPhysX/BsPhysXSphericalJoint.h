//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "Physics/BsSphericalJoint.h"
#include "PxPhysics.h"
#include "extensions/PxSphericalJoint.h"

namespace bs
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of a SphericalJoint. */
	class PhysXSphericalJoint : public SphericalJoint
	{
	public:
		PhysXSphericalJoint(physx::PxPhysics* physx, const SPHERICAL_JOINT_DESC& desc);
		~PhysXSphericalJoint();

		/** @copydoc SphericalJoint::getLimit */
		LimitConeRange getLimit() const override;

		/** @copydoc SphericalJoint::setLimit */
		void setLimit(const LimitConeRange& limit) override;

		/** @copydoc SphericalJoint::setFlag */
		void setFlag(SphericalJointFlag flag, bool enabled) override;

		/** @copydoc SphericalJoint::hasFlag */
		bool hasFlag(SphericalJointFlag flag) const override;

	private:
		/** Returns the internal PhysX representation of the spherical joint. */
		inline physx::PxSphericalJoint* getInternal() const;
	};

	/** @} */
}
