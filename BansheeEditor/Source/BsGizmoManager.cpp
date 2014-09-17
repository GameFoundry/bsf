#include "BsGizmoManager.h"
#include "BsMesh.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsVertexDataDesc.h"
#include "BsDrawHelper3D.h"
#include "BsMeshHeap.h"
#include "BsCamera.h"
#include "BsSpriteTexture.h"

namespace BansheeEngine
{
	const UINT32 GizmoManager::VERTEX_BUFFER_GROWTH = 4096;
	const UINT32 GizmoManager::INDEX_BUFFER_GROWTH = 4096 * 2;
	const UINT32 GizmoManager::SPHERE_QUALITY = 1;
	const float GizmoManager::MAX_ICON_RANGE = 500.0f;

	GizmoManager::GizmoManager(const HCamera& camera)
		:mTotalRequiredSolidIndices(0), mTotalRequiredSolidVertices(0),
		mTotalRequiredWireVertices(0), mTotalRequiredWireIndices(0), mCamera(camera), mPickable(false)
	{
		mSolidVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);
		mSolidVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mWireVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mWireVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mWireVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mIconVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mIconVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mIconVertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD);
		mIconVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mSolidMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mSolidVertexDesc);
		mWireMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mWireVertexDesc);
		mIconMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mIconVertexDesc);
	}

	GizmoManager::~GizmoManager()
	{
		if (mSolidMesh != nullptr)
			mSolidMeshHeap->dealloc(mSolidMesh);

		if (mWireMesh != nullptr)
			mWireMeshHeap->dealloc(mWireMesh);

		if (mIconMesh != nullptr)
			mIconMeshHeap->dealloc(mIconMesh);
	}

	void GizmoManager::startGizmo(const HSceneObject& gizmoParent)
	{
		mActiveSO = gizmoParent;
	}

	void GizmoManager::endGizmo()
	{
		mActiveSO = nullptr;
	}

	void GizmoManager::setColor(const Color& color)
	{
		mColor = color;
	}

	void GizmoManager::setTransform(const Matrix4& transform)
	{
		mTransform = transform;
	}

	void GizmoManager::drawCube(const Vector3& position, const Vector3& extents)
	{
		mSolidCubeData.push_back(CubeData());
		CubeData& cubeData = mSolidCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.sceneObject = mActiveSO;
		cubeData.pickable = mPickable;

		mTotalRequiredSolidVertices += 24;
		mTotalRequiredSolidIndices += 36;
	}

	void GizmoManager::drawSphere(const Vector3& position, float radius)
	{
		mSolidSphereData.push_back(SphereData());
		SphereData& sphereData = mSolidSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.sceneObject = mActiveSO;
		sphereData.pickable = mPickable;

		UINT32 numVertices, numIndices;
		DrawHelper3D::getNumElementsSphere(SPHERE_QUALITY, numVertices, numIndices);

		mTotalRequiredSolidVertices += numVertices;
		mTotalRequiredSolidIndices += numIndices;
	}

	void GizmoManager::drawWireCube(const Vector3& position, const Vector3& extents)
	{
		mWireCubeData.push_back(CubeData());
		CubeData& cubeData = mWireCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.sceneObject = mActiveSO;
		cubeData.pickable = mPickable;

		mTotalRequiredWireVertices += 8;
		mTotalRequiredWireIndices += 24;
	}

	void GizmoManager::drawWireSphere(const Vector3& position, float radius)
	{
		mWireSphereData.push_back(SphereData());
		SphereData& sphereData = mWireSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.sceneObject = mActiveSO;
		sphereData.pickable = mPickable;

		UINT32 numVertices, numIndices;
		DrawHelper3D::getNumElementsWireSphere(SPHERE_QUALITY, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void GizmoManager::drawLine(const Vector3& start, const Vector3& end)
	{
		mLineData.push_back(LineData());
		LineData& lineData = mLineData.back();

		lineData.start = start;
		lineData.end = end;
		lineData.color = mColor;
		lineData.transform = mTransform;
		lineData.sceneObject = mActiveSO;
		lineData.pickable = mPickable;

		mTotalRequiredWireVertices += 2;
		mTotalRequiredWireIndices += 2;
	}

	void GizmoManager::drawFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far)
	{
		mFrustumData.push_back(FrustumData());
		FrustumData& frustumData = mFrustumData.back();

		frustumData.position = position;
		frustumData.aspect = aspect;
		frustumData.FOV = FOV;
		frustumData.near = near;
		frustumData.far = far;
		frustumData.color = mColor;
		frustumData.transform = mTransform;
		frustumData.sceneObject = mActiveSO;
		frustumData.pickable = mPickable;

		mTotalRequiredWireVertices += 8;
		mTotalRequiredWireIndices += 36;
	}

	void GizmoManager::drawIcon(Vector3 position, HSpriteTexture image, bool fixedScale)
	{
		mIconData.push_back(IconData());
		IconData& iconData = mIconData.back();

		iconData.position = position;
		iconData.texture = image;
		iconData.fixedScale = fixedScale;
		iconData.color = mColor;
		iconData.transform = mTransform;
		iconData.sceneObject = mActiveSO;
		iconData.pickable = mPickable;
	}

	void GizmoManager::update()
	{
		buildSolidMesh();
		buildWireMesh();
		buildIconMesh();
	}

	void GizmoManager::clearGizmos()
	{
		mSolidCubeData.clear();
		mWireCubeData.clear();
		mSolidSphereData.clear();
		mWireSphereData.clear();
		mLineData.clear();
		mFrustumData.clear();
		mIconData.clear();

		mTotalRequiredSolidVertices = 0;
		mTotalRequiredSolidIndices = 0;
		mTotalRequiredWireVertices = 0;
		mTotalRequiredWireIndices = 0;
	}

	void GizmoManager::buildSolidMesh()
	{
		MeshDataPtr meshData = bs_shared_ptr<MeshData>(mTotalRequiredSolidVertices, mTotalRequiredSolidIndices, mSolidVertexDesc);

		UINT32 curVertexOffset = 0;
		UINT32 curIndexOffet = 0;

		auto positionIter = meshData->getVec3DataIter(VES_POSITION);
		auto normalIter = meshData->getVec3DataIter(VES_NORMAL);
		auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

		for (auto& cubeData : mSolidCubeData)
		{
			AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
			DrawHelper3D::instance().solidAABox(box, meshData, curVertexOffset, curIndexOffet); // TODO - These should be static methods

			Matrix4 transformIT = cubeData.transform.inverseAffine().transpose();
			RGBA color = cubeData.color.getAsRGBA();

			UINT32 numVertices = 24;
			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = cubeData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());
				
				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += 36;
		}

		UINT32 numSphereVertices, numSphereIndices;
		DrawHelper3D::getNumElementsSphere(SPHERE_QUALITY, numSphereVertices, numSphereIndices);

		for (auto& sphereData : mSolidSphereData)
		{
			Sphere sphere(sphereData.position, sphereData.radius);
			DrawHelper3D::instance().solidSphere(sphere, meshData, curVertexOffset, curIndexOffet, SPHERE_QUALITY);

			Matrix4 transformIT = sphereData.transform.inverseAffine().transpose();
			RGBA color = sphereData.color.getAsRGBA();

			for (UINT32 i = 0; i < numSphereVertices; i++)
			{
				Vector3 worldPos = sphereData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numSphereVertices;
			curIndexOffet += numSphereIndices;
		}

		if (mSolidMesh != nullptr)
			mSolidMeshHeap->dealloc(mSolidMesh);

		mSolidMesh = mSolidMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
	}

	void GizmoManager::buildWireMesh()
	{
		MeshDataPtr meshData = bs_shared_ptr<MeshData>(mTotalRequiredWireVertices, mTotalRequiredWireIndices, mWireVertexDesc);

		UINT32 curVertexOffset = 0;
		UINT32 curIndexOffet = 0;

		auto positionIter = meshData->getVec3DataIter(VES_POSITION);
		auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

		for (auto& cubeData : mWireCubeData)
		{
			AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
			DrawHelper3D::instance().wireAABox(box, meshData, curVertexOffset, curIndexOffet); // TODO - These should be static methods

			RGBA color = cubeData.color.getAsRGBA();

			UINT32 numVertices = 8;
			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = cubeData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += 24;
		}

		UINT32 numSphereVertices, numSphereIndices;
		DrawHelper3D::getNumElementsWireSphere(SPHERE_QUALITY, numSphereVertices, numSphereIndices);

		for (auto& sphereData : mWireSphereData)
		{
			Sphere sphere(sphereData.position, sphereData.radius);
			DrawHelper3D::instance().wireSphere(sphere, meshData, curVertexOffset, curIndexOffet, SPHERE_QUALITY);

			RGBA color = sphereData.color.getAsRGBA();
			for (UINT32 i = 0; i < numSphereVertices; i++)
			{
				Vector3 worldPos = sphereData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += numSphereVertices;
			curIndexOffet += numSphereIndices;
		}

		for (auto& lineData : mLineData)
		{
			DrawHelper3D::instance().pixelLine(lineData.start, lineData.end, meshData, curVertexOffset, curIndexOffet);

			RGBA color = lineData.color.getAsRGBA();
			for (UINT32 i = 0; i < 2; i++)
			{
				Vector3 worldPos = lineData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += 2;
			curIndexOffet += 2;
		}

		for (auto& frustumData : mFrustumData)
		{
			DrawHelper3D::instance().wireFrustum(frustumData.aspect, frustumData.FOV, frustumData.near, 
				frustumData.far, meshData, curVertexOffset, curIndexOffet);

			RGBA color = frustumData.color.getAsRGBA();
			for (UINT32 i = 0; i < 8; i++)
			{
				Vector3 worldPos = frustumData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += 8;
			curIndexOffet += 24;
		}

		if (mWireMesh != nullptr)
			mWireMeshHeap->dealloc(mWireMesh);

		mWireMesh = mWireMeshHeap->alloc(meshData, DOT_LINE_LIST);
	}

	void GizmoManager::buildIconMesh()
	{
		mSortedIconData.clear();
		
		if (mIconData.size() > mSortedIconData.size())
			mSortedIconData.resize(mIconData.size());

		UINT32 i = 0;
		for (auto& iconData : mIconData)
		{
			Vector3 viewPoint = mCamera->worldToViewPoint(iconData.position);

			float distance = -viewPoint.z;
			if (distance < mCamera->getNearClipDistance()) // Ignore behind clip plane
				continue;

			if (distance > MAX_ICON_RANGE) // Ignore too far away
				continue;

			if (!iconData.texture) // Ignore missing texture
				continue;

			SortedIconData& sortedIconData = mSortedIconData[i];
			sortedIconData.iconIdx = i;
			sortedIconData.distance = distance;
			sortedIconData.screenPosition = mCamera->viewToScreenPoint(viewPoint);

			i++;
		}

		UINT32 actualNumIcons = i;

		// Sort back to front first, then by texture
		std::sort(mSortedIconData.begin(), mSortedIconData.begin() + actualNumIcons, 
			[&](const SortedIconData& a, const SortedIconData& b)
		{
			if (a.distance == b.distance)
			{
				HSpriteTexture texA = mIconData[a.iconIdx].texture;
				HSpriteTexture texB = mIconData[b.iconIdx].texture;

				if (texA == texB)
					return a.iconIdx < b.iconIdx;

				return texA->getInternalID() < texB->getInternalID();
			}
			else
				return a.distance > b.distance;
		});

		for (i = 0; i < actualNumIcons; i++)
		{

		}

		// TODO - Group by texture
		// TODO - Scale icons to always fit in some max size
		// TODO - Render in screen space


		// TODO - Set up a render method that actually renders the gizmos
		//	      - Optionally also add a special method that renders the gizmos for picking purposes


		// ----------------------------
		// TODO - In SBansheeEditor:
		//  - Add BsScriptGizmos that implements the C# Gizmos interface
		//  - It tracks when an assembly is reloaded, and finds all valid gizmo drawing methods
		//     - They need to have DrawGizmo attribute and accept a Component of a specific type as first param (and be static)
		//     - Internally they call GizmoManager
		//  - Then in update:
		//   - Call GizmoManager and clear gizmos
		//   - Go through every SceneObject and their components to find custom gizmos for those types
		//   - Call their draw gizmo methods
		//
		// TODO - In ScenePicking:
		//  - Call GizmoManager that renders all gizmos using picking color coding
		//   - I might instead just add a way to retrieve render data from GizmoManager since GizmoManager doesn't have access to picking materials
		//
		// TODO - How do I handle C++ types like Camera and Renderable?
		//  - Add ComponentBase.cs from which Camera and Renderable inherit
		//  - Ensure any instances where I currently use Component I use ComponentBase instead (except for actual ManagedComponents)
		//  - Camera and Renderable will have their own specialized implementation for scripts, but interally I will use normal Camera/Renderable components
		//  - TODO Haven't thought this out yet
	}
}