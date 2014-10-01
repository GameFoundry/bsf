#include "BsDrawHelper.h"
#include "BsMesh.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsVertexDataDesc.h"
#include "BsMeshHeap.h"
#include "BsShapeMeshes3D.h"

namespace BansheeEngine
{
	const UINT32 DrawHelper::VERTEX_BUFFER_GROWTH = 4096;
	const UINT32 DrawHelper::INDEX_BUFFER_GROWTH = 4096 * 2;

	DrawHelper::DrawHelper()
		: mTotalRequiredSolidVertices(0), mTotalRequiredSolidIndices(0), 
		mTotalRequiredWireVertices(0), mTotalRequiredWireIndices(0)
	{
		mSolidVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);
		mSolidVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mWireVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mWireVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mWireVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mSolidMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mSolidVertexDesc);
		mWireMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mWireVertexDesc);
	}

	DrawHelper::~DrawHelper()
	{

	}

	void DrawHelper::setColor(const Color& color)
	{
		mColor = color;
	}

	void DrawHelper::setTransform(const Matrix4& transform)
	{
		mTransform = transform;
	}

	void DrawHelper::cube(const Vector3& position, const Vector3& extents)
	{
		mSolidCubeData.push_back(CubeData());
		CubeData& cubeData = mSolidCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;

		mTotalRequiredSolidVertices += 24;
		mTotalRequiredSolidIndices += 36;
	}

	void DrawHelper::sphere(const Vector3& position, float radius, UINT32 quality)
	{
		mSolidSphereData.push_back(SphereData());
		SphereData& sphereData = mSolidSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.quality = quality;
		sphereData.color = mColor;
		sphereData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsSphere(quality, numVertices, numIndices);

		mTotalRequiredSolidVertices += numVertices;
		mTotalRequiredSolidIndices += numIndices;
	}

	void DrawHelper::wireCube(const Vector3& position, const Vector3& extents)
	{
		mWireCubeData.push_back(CubeData());
		CubeData& cubeData = mWireCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;

		mTotalRequiredWireVertices += 8;
		mTotalRequiredWireIndices += 24;
	}

	void DrawHelper::wireSphere(const Vector3& position, float radius, UINT32 quality)
	{
		mWireSphereData.push_back(SphereData());
		SphereData& sphereData = mWireSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.quality = quality;
		sphereData.color = mColor;
		sphereData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsWireSphere(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::line(const Vector3& start, const Vector3& end)
	{
		mLineData.push_back(LineData());
		LineData& lineData = mLineData.back();

		lineData.start = start;
		lineData.end = end;
		lineData.color = mColor;
		lineData.transform = mTransform;

		mTotalRequiredWireVertices += 2;
		mTotalRequiredWireIndices += 2;
	}

	void DrawHelper::frustum(const Vector3& position, float aspect, Degree FOV, float near, float far)
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

		mTotalRequiredWireVertices += 8;
		mTotalRequiredWireIndices += 36;
	}

	void DrawHelper::cone(const Vector3& base, const Vector3& normal, float height, float radius, UINT32 quality)
	{
		mConeData.push_back(ConeData());
		ConeData& coneData = mConeData.back();

		coneData.base = base;
		coneData.normal = normal;
		coneData.height = height;
		coneData.radius = radius;
		coneData.quality = quality;
		coneData.color = mColor;
		coneData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsCone(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::disc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality)
	{
		mDiscData.push_back(DiscData());
		DiscData& discData = mDiscData.back();

		discData.position = position;
		discData.normal = normal;
		discData.radius = radius;
		discData.quality = quality;
		discData.color = mColor;
		discData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsDisc(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::wireDisc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality)
	{
		mWireDiscData.push_back(DiscData());
		DiscData& discData = mWireDiscData.back();

		discData.position = position;
		discData.normal = normal;
		discData.radius = radius;
		discData.quality = quality;
		discData.color = mColor;
		discData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsWireDisc(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::arc(const Vector3& position, const Vector3& normal, float radius, 
		Degree startAngle, Degree amountAngle, UINT32 quality)
	{
		mArcData.push_back(ArcData());
		ArcData& arcData = mArcData.back();

		arcData.position = position;
		arcData.normal = normal;
		arcData.radius = radius;
		arcData.startAngle = startAngle;
		arcData.amountAngle = amountAngle;
		arcData.quality = quality;
		arcData.color = mColor;
		arcData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsArc(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::wireArc(const Vector3& position, const Vector3& normal, float radius, 
		Degree startAngle, Degree amountAngle, UINT32 quality)
	{
		mWireArcData.push_back(ArcData());
		ArcData& arcData = mWireArcData.back();

		arcData.position = position;
		arcData.normal = normal;
		arcData.radius = radius;
		arcData.startAngle = startAngle;
		arcData.amountAngle = amountAngle;
		arcData.quality = quality;
		arcData.color = mColor;
		arcData.transform = mTransform;

		UINT32 numVertices, numIndices;
		ShapeMeshes3D::getNumElementsWireArc(quality, numVertices, numIndices);

		mTotalRequiredWireVertices += numVertices;
		mTotalRequiredWireIndices += numIndices;
	}

	void DrawHelper::rectangle(const Rect3& area)
	{
		mRect3Data.push_back(Rect3Data());
		Rect3Data& rectData = mRect3Data.back();

		rectData.area = area;
		rectData.color = mColor;
		rectData.transform = mTransform;

		mTotalRequiredWireVertices += 4;
		mTotalRequiredWireIndices += 6;
	}

	void DrawHelper::clear()
	{
		mSolidCubeData.clear();
		mWireCubeData.clear();
		mSolidSphereData.clear();
		mWireSphereData.clear();
		mLineData.clear();
		mRect3Data.clear();
		mFrustumData.clear();
		mFrustumData.clear();
		mDiscData.clear();
		mWireDiscData.clear();
		mArcData.clear();
		mWireArcData.clear();

		mTotalRequiredSolidVertices = 0;
		mTotalRequiredSolidIndices = 0;

		mTotalRequiredWireVertices = 0;
		mTotalRequiredWireIndices = 0;
	}

	TransientMeshPtr DrawHelper::buildSolidMesh()
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
			ShapeMeshes3D::solidAABox(box, meshData, curVertexOffset, curIndexOffet);

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

		for (auto& discData : mSolidSphereData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsSphere(discData.quality, numVertices, numIndices);

			Sphere sphere(discData.position, discData.radius);
			ShapeMeshes3D::solidSphere(sphere, meshData, curVertexOffset, curIndexOffet, discData.quality);

			Matrix4 transformIT = discData.transform.inverseAffine().transpose();
			RGBA color = discData.color.getAsRGBA();

			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = discData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& discData : mConeData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsCone(discData.quality, numVertices, numIndices);

			ShapeMeshes3D::solidCone(discData.base, discData.normal, discData.height, discData.radius, 
				meshData, curVertexOffset, curIndexOffet, discData.quality);

			Matrix4 transformIT = discData.transform.inverseAffine().transpose();
			RGBA color = discData.color.getAsRGBA();

			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = discData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& discData : mDiscData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsDisc(discData.quality, numVertices, numIndices);

			ShapeMeshes3D::solidDisc(discData.position, discData.radius, discData.normal,
				meshData, curVertexOffset, curIndexOffet, discData.quality);

			Matrix4 transformIT = discData.transform.inverseAffine().transpose();
			RGBA color = discData.color.getAsRGBA();

			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = discData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& arcData : mArcData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsArc(arcData.quality, numVertices, numIndices);

			ShapeMeshes3D::solidArc(arcData.position, arcData.radius, arcData.normal, 
				arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

			Matrix4 transformIT = arcData.transform.inverseAffine().transpose();
			RGBA color = arcData.color.getAsRGBA();

			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = arcData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& rectData : mRect3Data)
		{
			ShapeMeshes3D::solidQuad(rectData.area, meshData, curVertexOffset, curIndexOffet);

			Matrix4 transformIT = rectData.transform.inverseAffine().transpose();
			RGBA color = rectData.color.getAsRGBA();

			for (UINT32 i = 0; i < 4; i++)
			{
				Vector3 worldPos = rectData.transform.multiply3x4(positionIter.getValue());
				Vector3 worldNormal = transformIT.multiply3x4(normalIter.getValue());

				positionIter.addValue(worldPos);
				normalIter.addValue(worldNormal);
				colorIter.addValue(color);
			}

			curVertexOffset += 4;
			curIndexOffet += 6;
		}

		return mSolidMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
	}

	TransientMeshPtr DrawHelper::buildWireMesh()
	{
		MeshDataPtr meshData = bs_shared_ptr<MeshData>(mTotalRequiredWireVertices, 
			mTotalRequiredWireIndices, mWireVertexDesc);

		UINT32 curVertexOffset = 0;
		UINT32 curIndexOffet = 0;

		auto positionIter = meshData->getVec3DataIter(VES_POSITION);
		auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

		for (auto& cubeData : mWireCubeData)
		{
			AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
			ShapeMeshes3D::wireAABox(box, meshData, curVertexOffset, curIndexOffet);

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

		for (auto& sphereData : mWireSphereData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsWireSphere(sphereData.quality, numVertices, numIndices);

			Sphere sphere(sphereData.position, sphereData.radius);
			ShapeMeshes3D::wireSphere(sphere, meshData, curVertexOffset, curIndexOffet, sphereData.quality);

			RGBA color = sphereData.color.getAsRGBA();
			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = sphereData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& lineData : mLineData)
		{
			ShapeMeshes3D::pixelLine(lineData.start, lineData.end, meshData, curVertexOffset, curIndexOffet);

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
			ShapeMeshes3D::wireFrustum(frustumData.aspect, frustumData.FOV, frustumData.near,
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

		for (auto& discData : mWireDiscData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsDisc(discData.quality, numVertices, numIndices);

			ShapeMeshes3D::solidDisc(discData.position, discData.radius, discData.normal,
				meshData, curVertexOffset, curIndexOffet, discData.quality);

			RGBA color = discData.color.getAsRGBA();
			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = discData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		for (auto& arcData : mWireArcData)
		{
			UINT32 numVertices, numIndices;
			ShapeMeshes3D::getNumElementsArc(arcData.quality, numVertices, numIndices);

			ShapeMeshes3D::solidArc(arcData.position, arcData.radius, arcData.normal,
				arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

			RGBA color = arcData.color.getAsRGBA();
			for (UINT32 i = 0; i < numVertices; i++)
			{
				Vector3 worldPos = arcData.transform.multiply3x4(positionIter.getValue());

				positionIter.addValue(worldPos);
				colorIter.addValue(color);
			}

			curVertexOffset += numVertices;
			curIndexOffet += numIndices;
		}

		return mWireMeshHeap->alloc(meshData, DOT_LINE_LIST);
	}

	void DrawHelper::releaseSolidMesh(const TransientMeshPtr& mesh)
	{
		mSolidMeshHeap->dealloc(mesh);
	}

	void DrawHelper::releaseWireMesh(const TransientMeshPtr& mesh)
	{
		mWireMeshHeap->dealloc(mesh);
	}
}