#include "BsPhysicsMesh.h"
#include "BsPhysicsMeshRTTI.h"
#include "BsResources.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	PhysicsMesh::PhysicsMesh(const MeshDataPtr& meshData, PhysicsMeshType type)
		:mInitMeshData(meshData), mType(type)
	{ }

	HPhysicsMesh PhysicsMesh::create(const MeshDataPtr& meshData, PhysicsMeshType type)
	{
		PhysicsMeshPtr newMesh = _createPtr(meshData, type);

		return static_resource_cast<PhysicsMesh>(gResources()._createResourceHandle(newMesh));
	}

	PhysicsMeshPtr PhysicsMesh::_createPtr(const MeshDataPtr& meshData, PhysicsMeshType type)
	{
		PhysicsMeshPtr newMesh = gPhysics().createMesh(meshData, type);
		newMesh->_setThisPtr(newMesh);
		newMesh->initialize();

		return newMesh;
	}

	void PhysicsMesh::initialize()
	{
		mInitMeshData = nullptr;

		Resource::initialize();
	}

	RTTITypeBase* PhysicsMesh::getRTTIStatic()
	{
		return PhysicsMeshRTTI::instance();
	}

	RTTITypeBase* PhysicsMesh::getRTTI() const
	{
		return getRTTIStatic();
	}
}
