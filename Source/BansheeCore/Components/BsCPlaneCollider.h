//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "Physics/BsPlaneCollider.h"
#include "Components/BsCCollider.h"

namespace bs 
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	PlaneCollider
	 *
	 * @note Wraps PlaneCollider as a Component.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Physics,n:PlaneCollider) CPlaneCollider : public CCollider
	{
	public:
		CPlaneCollider(const HSceneObject& parent);

		/** Normal vector that determines the local orientation of the plane. */
		BS_SCRIPT_EXPORT(n:Normal,pr:setter)
		void setNormal(const Vector3& normal);

		/** @copydoc setNormal() */
		BS_SCRIPT_EXPORT(n:Normal,pr:getter)
		Vector3 getNormal() const { return mNormal; }

		/** Determines the distance of the plane from the local origin, along its normal vector. */
		BS_SCRIPT_EXPORT(n:Distance,pr:setter)
		void setDistance(float distance);

		/** @copydoc setDistance() */
		BS_SCRIPT_EXPORT(n:Distance,pr:getter)
		float getDistance() const { return mDistance; }

		/** @name Internal
		 *  @{
		 */

		/**	Returns the plane collider that this component wraps. */
		PlaneCollider* _getInternal() const { return static_cast<PlaneCollider*>(mInternal.get()); }

		/** @} */

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/** @copydoc CCollider::createInternal */
		SPtr<Collider> createInternal() override;

		/** @copydoc CCollider::isValidParent */
		bool isValidParent(const HRigidbody& parent) const override;

	protected:
		Vector3 mNormal = Vector3::UNIT_X;
		float mDistance = 0.0f;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CPlaneColliderRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CPlaneCollider() {} // Serialization only
	};

	 /** @} */
}