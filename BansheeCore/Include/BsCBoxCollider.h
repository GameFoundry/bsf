//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsBoxCollider.h"
#include "BsCCollider.h"

namespace BansheeEngine 
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	BoxCollider
	 *
	 * Wraps BoxCollider as a Component.
	 */
    class BS_CORE_EXPORT CBoxCollider : public CCollider
    {
    public:
		CBoxCollider(const HSceneObject& parent, const Vector3& extents = Vector3::ONE);

		/** @copydoc BoxCollider::setExtents */
		void setExtents(const Vector3& extents);

		/** @copydoc BoxCollider::getExtents */
		Vector3 getExtents() const { return mExtents; }

		/** Sets the position of the box shape, relative to the component's scene object. */
		void setCenter(const Vector3& center);

		/** Gets the position of the box shape, relative to the component's scene object. */
		Vector3 getCenter() const { return mLocalPosition; }

	    /**	Returns the box collider that this component wraps. */
		SPtr<BoxCollider> _getInternal() const { return std::static_pointer_cast<BoxCollider>(mInternal); }

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/** @copydoc CCollider::createInternal */
		SPtr<Collider> createInternal() override;

    protected:
		Vector3 mExtents;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CBoxColliderRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CBoxCollider() {} // Serialization only
     };

	 /** @} */
}