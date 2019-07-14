//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "Physics/BsDistanceJoint.h"
#include "PxPhysics.h"
#include "extensions/PxDistanceJoint.h"

namespace bs
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of a DistanceJoint */
	class PhysXDistanceJoint : public DistanceJoint
	{
	public:
		PhysXDistanceJoint(physx::PxPhysics* physx, const DISTANCE_JOINT_DESC& desc);
		~PhysXDistanceJoint();

		/** @copydoc DistanceJoint::getDistance */
		float getDistance() const override;

		/** @copydoc DistanceJoint::getMinDistance */
		float getMinDistance() const override;

		/** @copydoc DistanceJoint::setMinDistance */
		void setMinDistance(float value) override;

		/** @copydoc DistanceJoint::getMaxDistance */
		float getMaxDistance() const override;

		/** @copydoc DistanceJoint::setMaxDistance */
		void setMaxDistance(float value) override;

		/** @copydoc DistanceJoint::getTolerance */
		float getTolerance() const override;

		/** @copydoc DistanceJoint::setTolerance */
		void setTolerance(float value) override;

		/** @copydoc DistanceJoint::getSpring */
		Spring getSpring() const override;

		/** @copydoc DistanceJoint::setSpring */
		void setSpring(const Spring& value) override;

		/** @copydoc DistanceJoint::setFlag */
		void setFlag(DistanceJointFlag flag, bool enabled) override;

		/** @copydoc DistanceJoint::hasFlag */
		bool hasFlag(DistanceJointFlag flag) const override;

	private:
		/** Returns the internal PhysX representation of the distance joint. */
		inline physx::PxDistanceJoint* getInternal() const;
	};

	/** @} */
}
