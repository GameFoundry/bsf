//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsFixedJoint.h"
#include "BsCJoint.h"

namespace BansheeEngine 
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	FixedJoint
	 *
	 * Wraps FixedJoint as a Component.
	 */
    class BS_CORE_EXPORT CFixedJoint : public CJoint
    {
    public:
		CFixedJoint(const HSceneObject& parent);

		/** @cond INTERNAL */

	    /**	Returns the fixed joint that this component wraps. */
		FixedJoint* _getInternal() const { return static_cast<FixedJoint*>(mInternal.get()); }

		/** @endcond */

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/** @copydoc CJoint::createInternal */
		SPtr<Joint> createInternal() override;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CFixedJointRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CFixedJoint() {} // Serialization only
     };

	 /** @} */
}