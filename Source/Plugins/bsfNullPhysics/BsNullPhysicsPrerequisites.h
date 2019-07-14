//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
/** @addtogroup Plugins
 *  @{
 */

/** @defgroup NullPhysics bsfNullPhysics
 *	Null implementation of framework's physics.
 *  @{
 */

/** @cond RTTI */
/** @defgroup RTTI-Impl-NullPhysics RTTI types
 *  Types containing RTTI for specific classes.
 */
/** @endcond */

/** @} */
/** @} */

	class NullPhysicsRigidbody;
	class NullPhysicsMaterial;
	class FNullPhysicsCollider;

	/** @addtogroup NullPhysics
	 *  @{
	 */

	/**	Type IDs used by the RTTI system for the null physics library. */
	enum TypeID_NullPhysics
	{
		TID_FNullPhysicsMesh = 110000,
	};

	/** @} */
}
