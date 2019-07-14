//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Math/BsVector3.h"
#include "Math/BsQuaternion.h"

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/** Specifies first or second body referenced by a Joint. */
	enum class BS_SCRIPT_EXPORT(m:Physics) JointBody
	{
		Target, /**< Body the joint is influencing. */
		Anchor /**< Body the joint is attached to (if any). */
	};

	/** @} */
	/** @addtogroup Physics-Internal
	 *  @{
	 */

	/** Provides common functionality used by all Joint types. */
	class BS_CORE_EXPORT FJoint
	{
	public:
		FJoint(const JOINT_DESC& desc) { }
		virtual ~FJoint() = default;

		/** @copydoc setBody() */
		virtual Rigidbody* getBody(JointBody body) const = 0;

		/** Determines a body managed by the joint. One of the bodies must be movable (non-kinematic). */
		virtual void setBody(JointBody body, Rigidbody* value) = 0;

		/** Returns the position relative to the body, at which the body is anchored to the joint. */
		virtual Vector3 getPosition(JointBody body) const = 0;

		/** Returns the rotation relative to the body, at which the body is anchored to the joint. */
		virtual Quaternion getRotation(JointBody body) const = 0;

		/** Sets the position and rotation relative to the body, at which the body is anchored to the joint.  */
		virtual void setTransform(JointBody body, const Vector3& position, const Quaternion& rotation) = 0;

		/** @copydoc setBreakForce() */
		virtual float getBreakForce() const = 0;

		/**
		 * Determines the maximum force the joint can apply before breaking. Broken joints no longer participate in physics
		 * simulation.
		 */
		virtual void setBreakForce(float force) = 0;

		/** @copydoc setBreakTorque() */
		virtual float getBreakTorque() const = 0;

		/**
		 * Determines the maximum torque the joint can apply before breaking. Broken joints no longer participate in physics
		 * simulation.
		 */
		virtual void setBreakTorque(float torque) = 0;

		/** @copydoc setEnableCollision() */
		virtual bool getEnableCollision() const = 0;

		/** Determines whether collision between the two bodies managed by the joint are enabled. */
		virtual void setEnableCollision(bool value) = 0;
	};

	/** @} */
}
