//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullPhysicsPrerequisites.h"
#include "Physics/BsPhysicsMesh.h"

namespace bs
{
	/** @addtogroup NullPhysics
	 *  @{
	 */

	/** Null implementation of a PhysicsMesh. */
	class NullPhysicsMesh : public PhysicsMesh
	{
	public:
		NullPhysicsMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type);

	private:
		/** @copydoc PhysicsMesh::initialize() */
		void initialize() override;

		/** @copydoc PhysicsMesh::initialize() */
		void destroy() override;

		// Note: Must not have its own RTTI type, it's important it shares the same type ID as PhysicsMesh so the
		// system knows to recognize it. Use FPhysicsMesh instead.
	};

	/** Null implementation of the PhysicsMesh foundation, FPhysicsMesh. */
	class FNullPhysicsMesh : public FPhysicsMesh
	{
	public:
		FNullPhysicsMesh(const SPtr<MeshData>& meshData, PhysicsMeshType type);
		~FNullPhysicsMesh() = default;

		/** @copydoc PhysicsMesh::getMeshData */
		SPtr<MeshData> getMeshData() const override;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		FNullPhysicsMesh(); // Serialization only

		friend class FNullPhysicsMeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}
