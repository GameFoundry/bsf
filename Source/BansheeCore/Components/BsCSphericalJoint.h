//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "Physics/BsSphericalJoint.h"
#include "Components/BsCJoint.h"

namespace bs 
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	SphericalJoint
	 *
	 * @note	Wraps SphericalJoint as a Component.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Physics,n:SphericalJoint) CSphericalJoint : public CJoint
	{
	public:
		CSphericalJoint(const HSceneObject& parent);

		/** @copydoc SphericalJoint::getLimit */
		BS_SCRIPT_EXPORT(n:Limit,pr:getter)
		LimitConeRange getLimit() const;

		/** @copydoc SphericalJoint::setLimit */
		BS_SCRIPT_EXPORT(n:Limit,pr:setter)
		void setLimit(const LimitConeRange& limit);

		/** @copydoc SphericalJoint::setFlag */
		BS_SCRIPT_EXPORT(n:SetFlag)
		void setFlag(SphericalJointFlag flag, bool enabled);

		/** @copydoc SphericalJoint::hasFlag */
		BS_SCRIPT_EXPORT(n:HasFlag)
		bool hasFlag(SphericalJointFlag flag) const;

		/** @name Internal
		 *  @{
		 */

		/**	Returns the spherical joint that this component wraps. */
		SphericalJoint* _getInternal() const { return static_cast<SphericalJoint*>(mInternal.get()); }

		/** @} */

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/** @copydoc CJoint::createInternal */
		SPtr<Joint> createInternal() override;

		SPHERICAL_JOINT_DESC mDesc;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CSphericalJointRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CSphericalJoint(); // Serialization only
	};

	 /** @} */
}