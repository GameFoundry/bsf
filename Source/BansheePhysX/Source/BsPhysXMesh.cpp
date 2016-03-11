#include "BsPhysXMesh.h"
#include "BsPhysXMeshRTTI.h"
#include "BsMeshData.h"
#include "BsVertexDataDesc.h"
#include "BsPhysX.h"
#include "BsAABox.h"
#include "foundation\PxAllocatorCallback.h"
#include "geometry\PxTriangleMesh.h"
#include "geometry\PxConvexMesh.h"
#include "cooking\PxConvexMeshDesc.h"
#include "extensions\PxDefaultStreams.h"

using namespace physx;

namespace BansheeEngine
{
	/**
	 * Attempts to cook a convex mesh from the provided mesh data. Assumes the mesh data is not null and contains vertex
	 * positions as well as face indices. If the method returns true the resulting convex mesh will be output in the @p
	 * data buffer, and its size in @p size. The data buffer will be allocated used the generic allocator and is up to the
	 * caller to free it.
	 */
	bool cookConvex(PxCooking* cooking, const MeshDataPtr& meshData, UINT8** data, UINT32& size)
	{
		VertexDataDescPtr vertexDesc = meshData->getVertexDesc();

		// Generate hull polygons
		PxSimpleTriangleMesh meshDesc;
		meshDesc.points.count = meshData->getNumVertices();
		meshDesc.points.stride = vertexDesc->getVertexStride();
		meshDesc.points.data = meshData->getElementData(VES_POSITION);

		meshDesc.triangles.count = meshData->getNumIndices() / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);

		IndexType indexType = meshData->getIndexType();
		if (indexType == IT_32BIT)
		{
			meshDesc.triangles.stride = 4;
			meshDesc.triangles.data = meshData->getIndices32();
		}
		else
		{
			meshDesc.triangles.stride = 2;
			meshDesc.triangles.data = meshData->getIndices16();
			meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;
		}

		PxAllocatorCallback& allocator = PxGetFoundation().getAllocatorCallback();

		PxU32 numVertices = 0;
		PxU32 numIndices = 0;
		PxU32 numPolygons = 0;
		PxVec3* vertices = nullptr;
		PxU32* indices = nullptr;
		PxHullPolygon* polygons = nullptr;

		bool gotPolygons = cooking->computeHullPolygons(meshDesc, allocator,
			numVertices, vertices, numIndices, indices, numPolygons, polygons);

		// If we have polygons try to create hull directly from them
		if(gotPolygons)
		{
			PxConvexMeshDesc convexPolyDesc;
			convexPolyDesc.points.count = numVertices;
			convexPolyDesc.points.stride = sizeof(PxVec3);
			convexPolyDesc.points.data = vertices;

			convexPolyDesc.indices.count = numIndices;
			convexPolyDesc.indices.stride = sizeof(PxU32);
			convexPolyDesc.indices.data = indices;

			convexPolyDesc.polygons.count = numPolygons;
			convexPolyDesc.polygons.stride = sizeof(PxHullPolygon);
			convexPolyDesc.polygons.data = polygons;

			PxDefaultMemoryOutputStream output;
			if (cooking->cookConvexMesh(convexPolyDesc, output))
			{
				size = output.getSize();
				*data = (UINT8*)bs_alloc(size);

				memcpy(*data, output.getData(), size);

				allocator.deallocate(vertices);
				allocator.deallocate(indices);
				allocator.deallocate(polygons);

				return true;
			}

			allocator.deallocate(vertices);
			allocator.deallocate(indices);
			allocator.deallocate(polygons);
		}

		// Try to create hull from points
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count = meshData->getNumVertices();
		convexDesc.points.stride = vertexDesc->getVertexStride();
		convexDesc.points.data = meshData->getElementData(VES_POSITION);
		convexDesc.flags |= PxConvexFlag::eCOMPUTE_CONVEX;

		PxDefaultMemoryOutputStream output;
		if (cooking->cookConvexMesh(convexDesc, output))
		{
			size = output.getSize();
			*data = (UINT8*)bs_alloc(size);

			memcpy(*data, output.getData(), size);
			return true;
		}

		// Try inflating the convex mesh
		convexDesc.flags |= PxConvexFlag::eINFLATE_CONVEX;
		if (cooking->cookConvexMesh(convexDesc, output))
		{
			size = output.getSize();
			*data = (UINT8*)bs_alloc(size);

			memcpy(*data, output.getData(), size);
			return true;
		}

		// Nothing works, just compute an AABB
		AABox box;

		auto vertIter = meshData->getVec3DataIter(VES_POSITION);
		do
		{
			box.merge(vertIter.getValue());
		}
		while (vertIter.moveNext());

		Vector3 aabbVerts[8];
		aabbVerts[0] = box.getCorner(AABox::FAR_LEFT_BOTTOM);
		aabbVerts[1] = box.getCorner(AABox::FAR_RIGHT_BOTTOM);
		aabbVerts[2] = box.getCorner(AABox::FAR_RIGHT_TOP);
		aabbVerts[3] = box.getCorner(AABox::FAR_LEFT_TOP);

		aabbVerts[4] = box.getCorner(AABox::NEAR_LEFT_BOTTOM);
		aabbVerts[5] = box.getCorner(AABox::NEAR_RIGHT_BOTTOM);
		aabbVerts[6] = box.getCorner(AABox::NEAR_RIGHT_TOP);
		aabbVerts[7] = box.getCorner(AABox::NEAR_LEFT_TOP);

		convexDesc.points.count = 8;
		convexDesc.points.stride = sizeof(Vector3);
		convexDesc.points.data = &aabbVerts[0];
		convexDesc.flags &= ~PxConvexFlag::eINFLATE_CONVEX;

		if (cooking->cookConvexMesh(convexDesc, output))
		{
			size = output.getSize();
			*data = (UINT8*)bs_alloc(size);

			memcpy(*data, output.getData(), size);
			return true;
		}

		return false;
	}

	/**
	 * Attempts to cook a triangle or convex mesh from the provided mesh data. Will log a warning and return false if it is
	 * unable to cook the mesh. If the method returns true the resulting convex mesh will be output in the @p data buffer,
	 * and its size in @p size. The data buffer will be allocated used the generic allocator and is up to the caller to 
	 * free it.
	 */
	bool cookMesh(const MeshDataPtr& meshData, PhysicsMeshType type, UINT8** data, UINT32& size)
	{
		if (meshData == nullptr)
			return false;

		PxCooking* cooking = gPhysX().getCooking();
		if (cooking == nullptr)
		{
			LOGWRN("Attempting to cook a physics mesh but cooking is not enabled globally.");
			return false;
		}

		VertexDataDescPtr vertexDesc = meshData->getVertexDesc();
		if (!vertexDesc->hasElement(VES_POSITION))
		{
			LOGWRN("Provided PhysicsMesh mesh data has no vertex positions.");
			return false;
		}

		if (type == PhysicsMeshType::Convex)
		{
			if(!cookConvex(cooking, meshData, data, size))
			{
				LOGWRN("Failed cooking a convex mesh. Perpahs it is too complex? Maximum number of convex vertices is 256.");
				return false;
			}
		}
		else
		{
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = meshData->getNumVertices();
			meshDesc.points.stride = vertexDesc->getVertexStride();
			meshDesc.points.data = meshData->getElementData(VES_POSITION);

			meshDesc.triangles.count = meshData->getNumIndices() / 3;
			
			IndexType indexType = meshData->getIndexType();
			if (indexType == IT_32BIT)
			{
				meshDesc.triangles.stride = 3 * sizeof(PxU32);
				meshDesc.triangles.data = meshData->getIndices32();
			}
			else
			{
				meshDesc.triangles.stride = 3 * sizeof(PxU16);
				meshDesc.triangles.data = meshData->getIndices16();
				meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;
			}

			PxDefaultMemoryOutputStream output;
			if (!cooking->cookTriangleMesh(meshDesc, output))
				return false;

			size = output.getSize();
			*data = (UINT8*)bs_alloc(size);

			memcpy(*data, output.getData(), size);
		}

		return true;
	}

	PhysXMesh::PhysXMesh(const MeshDataPtr& meshData, PhysicsMeshType type)
		:PhysicsMesh(meshData, type)
	{ }

	void PhysXMesh::initialize()
	{
		if(mInternal == nullptr) // Could be not-null if we're deserializing
			mInternal = bs_shared_ptr_new<FPhysXMesh>(mInitMeshData, mType);

		PhysicsMesh::initialize();
	}

	void PhysXMesh::destroy()
	{
		mInternal = nullptr;

		PhysicsMesh::destroy();
	}

	FPhysXMesh::FPhysXMesh()
		:FPhysicsMesh(nullptr, PhysicsMeshType::Convex)
	{
		
	}

	FPhysXMesh::FPhysXMesh(const MeshDataPtr& meshData, PhysicsMeshType type)
		:FPhysicsMesh(meshData, type)
	{
		// Perform cooking if needed
		if (meshData != nullptr)
			cookMesh(meshData, mType, &mCookedData, mCookedDataSize);

		initialize();
	}

	FPhysXMesh::~FPhysXMesh()
	{
		if (mCookedData != nullptr)
		{
			bs_free(mCookedData);

			mCookedData = nullptr;
			mCookedDataSize = 0;
		}

		if (mTriangleMesh != nullptr)
		{
			mTriangleMesh->release();
			mTriangleMesh = nullptr;
		}

		if (mConvexMesh != nullptr)
		{
			mConvexMesh->release();
			mConvexMesh = nullptr;
		}
	}

	void FPhysXMesh::initialize()
	{
		if (mCookedData != nullptr && mCookedDataSize > 0)
		{
			PxPhysics* physx = gPhysX().getPhysX();

			PxDefaultMemoryInputData input(mCookedData, mCookedDataSize);
			if (mType == PhysicsMeshType::Convex)
				mConvexMesh = physx->createConvexMesh(input);
			else
				mTriangleMesh = physx->createTriangleMesh(input);
		}
	}

	MeshDataPtr FPhysXMesh::getMeshData() const
	{
		VertexDataDescPtr vertexDesc = VertexDataDesc::create();
		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		if (mConvexMesh == nullptr && mTriangleMesh == nullptr)
			return MeshData::create(0, 0, vertexDesc);

		UINT32 numVertices = 0;
		UINT32 numIndices = 0;

		if(mConvexMesh != nullptr)
		{
			numVertices = mConvexMesh->getNbVertices();

			UINT32 numPolygons = mConvexMesh->getNbPolygons();
			for (UINT32 i = 0; i < numPolygons; i++)
			{
				PxHullPolygon face;
				bool status = mConvexMesh->getPolygonData(i, face);
				assert(status);

				numIndices += (face.mNbVerts - 2) * 3;
			}
		}
		else // Triangle
		{
			numVertices = mTriangleMesh->getNbVertices();
			numIndices = mTriangleMesh->getNbTriangles() * 3;
		}

		MeshDataPtr meshData = MeshData::create(numVertices, numIndices, vertexDesc);

		auto posIter = meshData->getVec3DataIter(VES_POSITION);
		UINT32* outIndices = meshData->getIndices32();

		if (mConvexMesh != nullptr)
		{
			const PxVec3* convexVertices = mConvexMesh->getVertices();
			const UINT8* convexIndices = mConvexMesh->getIndexBuffer();

			UINT32 numPolygons = mConvexMesh->getNbPolygons();
			UINT32 offset = 0;
			for (UINT32 i = 0; i < numPolygons; i++)
			{
				PxHullPolygon face;
				bool status = mConvexMesh->getPolygonData(i, face);
				assert(status);

				const PxU8* faceIndices = convexIndices + face.mIndexBase;
				for (UINT32 j = 0; j < face.mNbVerts; j++)
					posIter.addValue(fromPxVector(convexVertices[faceIndices[j]]));

				for (UINT32 j = 2; j < face.mNbVerts; j++)
				{
					*outIndices++ = offset;
					*outIndices++ = offset + j;
					*outIndices++ = offset + j - 1;
				}

				offset += face.mNbVerts;
			}
		}
		else
		{
			const PxVec3* vertices = mTriangleMesh->getVertices();
			for (UINT32 i = 0; i < numVertices; i++)
				posIter.addValue(fromPxVector(vertices[i]));

			if(mTriangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
			{
				const UINT16* indices = (const UINT16*)mTriangleMesh->getTriangles();
				for (UINT32 i = 0; i < numIndices; i++)
					outIndices[i] = (UINT32)indices[i];
			}
			else
			{
				const UINT32* indices = (const UINT32*)mTriangleMesh->getTriangles();
				memcpy(outIndices, indices, numIndices * sizeof(UINT32));
			}
		}

		return meshData;
	}

	RTTITypeBase* FPhysXMesh::getRTTIStatic()
	{
		return FPhysXMeshRTTI::instance();
	}

	RTTITypeBase* FPhysXMesh::getRTTI() const
	{
		return getRTTIStatic();
	}
}