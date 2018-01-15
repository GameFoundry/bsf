//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "Physics/BsMeshCollider.h"
#include "Components/BsCCollider.h"

namespace bs 
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	MeshCollider
	 *
	 * @note Wraps MeshCollider as a Component.
	 */
	class BS_CORE_EXPORT BS_SCRIPT_EXPORT(m:Physics,n:MeshCollider) CMeshCollider : public CCollider
	{
	public:
		CMeshCollider(const HSceneObject& parent);

		/** @copydoc MeshCollider::setMesh */
		BS_SCRIPT_EXPORT(n:Mesh,pr:setter)
		void setMesh(const HPhysicsMesh& mesh);

		/** @copydoc MeshCollider::getMesh */
		BS_SCRIPT_EXPORT(n:Mesh,pr:getter)
		HPhysicsMesh getMesh() const { return mMesh; }

		/** @name Internal
		 *  @{
		 */

		/**	Returns the mesh collider that this component wraps. */
		MeshCollider* _getInternal() const { return static_cast<MeshCollider*>(mInternal.get()); }

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
		HPhysicsMesh mMesh;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CMeshColliderRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CMeshCollider(); // Serialization only
	};

	 /** @} */
}