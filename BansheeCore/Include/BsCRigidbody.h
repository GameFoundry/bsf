//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRigidbody.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	/** @addtogroup Components-Core
	 *  @{
	 */

	/**
	 * @copydoc	Rigidbody
	 *
	 * Wraps Rigidbody as a Component.
	 */
	class BS_CORE_EXPORT CRigidbody : public Component
	{
	public:
		CRigidbody(const HSceneObject& parent);

		// TODO
		SPtr<Rigidbody> _getInternal() const { return mInternal; }
		void _updateMassDistribution() { }
	protected:
		SPtr<Rigidbody> mInternal;

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CRigidbodyRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CRigidbody() {} // Serialization only
	};

	/** @} */
}