//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PhysicsMesh : public Resource
	{
	public:
		PhysicsMesh(const MeshDataPtr& meshData, PhysicsMeshType type);
		virtual ~PhysicsMesh() { }

		PhysicsMeshType getType() const { return mType; }
		virtual MeshDataPtr getMeshData() const = 0;

		static HPhysicsMesh create(const MeshDataPtr& meshData, PhysicsMeshType type = PhysicsMeshType::Convex);
		static PhysicsMeshPtr _createPtr(const MeshDataPtr& meshData, PhysicsMeshType type);

	protected:
		/** @copydoc PhysicsMesh::initialize() */
		void initialize() override;

		PhysicsMeshType mType;

		MeshDataPtr mInitMeshData; // Transient, only used during initalization

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PhysicsMeshRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};
}