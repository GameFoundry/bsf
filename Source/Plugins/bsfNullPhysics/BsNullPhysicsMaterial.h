//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullPhysicsPrerequisites.h"
#include "Physics/BsPhysicsMaterial.h"

namespace bs
{
	/** @addtogroup NullPhysics
	 *  @{
	 */

	/** Null implementation of a PhysicsMaterial. */
	class NullPhysicsMaterial : public PhysicsMaterial
	{
	public:
		NullPhysicsMaterial(float staFric, float dynFriction, float restitution);
		~NullPhysicsMaterial() = default;

		/** @copydoc PhysicsMaterial::setStaticFriction */
		void setStaticFriction(float value) override { mStaticFriction = value; }

		/** @copydoc PhysicsMaterial::getStaticFriction */
		float getStaticFriction() const override { return mStaticFriction; }

		/** @copydoc PhysicsMaterial::setDynamicFriction */
		void setDynamicFriction(float value) override { mDynamicFriction = value; }

		/** @copydoc PhysicsMaterial::getDynamicFriction */
		float getDynamicFriction() const override { return mDynamicFriction; }

		/** @copydoc PhysicsMaterial::setRestitutionCoefficient */
		void setRestitutionCoefficient(float value) override { mRestitutionCoefficient = value; }

		/** @copydoc PhysicsMaterial::getRestitutionCoefficient */
		float getRestitutionCoefficient() const override { return mRestitutionCoefficient; }

	private:
		float mStaticFriction;
		float mDynamicFriction;
		float mRestitutionCoefficient;
	};

	/** @} */
}
