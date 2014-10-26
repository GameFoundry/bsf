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
	{
		mTransform = Matrix4::IDENTITY;

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
		clearMeshes();
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
		cubeData.center = mTransform.multiplyAffine(position);
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
		sphereData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::wireCube(const Vector3& position, const Vector3& extents)
	{
		mWireCubeData.push_back(CubeData());
		CubeData& cubeData = mWireCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.center = mTransform.multiplyAffine(position);
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
		sphereData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::line(const Vector3& start, const Vector3& end)
	{
		mLineData.push_back(LineData());
		LineData& lineData = mLineData.back();

		lineData.start = start;
		lineData.end = end;
		lineData.color = mColor;
		lineData.transform = mTransform;
		lineData.center = mTransform.multiplyAffine((start + end) * 0.5f);
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
		frustumData.center = mTransform.multiplyAffine(position);
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
		coneData.center = mTransform.multiplyAffine(base + normal * height * 0.5f);
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
		discData.center = mTransform.multiplyAffine(position);
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
		discData.center = mTransform.multiplyAffine(position);
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
		arcData.center = mTransform.multiplyAffine(position);
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
		arcData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::rectangle(const Rect3& area)
	{
		mRect3Data.push_back(Rect3Data());
		Rect3Data& rectData = mRect3Data.back();

		rectData.area = area;
		rectData.color = mColor;
		rectData.transform = mTransform;
		rectData.center = mTransform.multiplyAffine(area.getCenter());
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
		mConeData.clear();
	}

	void DrawHelper::buildMeshes(SortType sorting, const Vector3& reference)
	{
		clearMeshes();

		enum class ShapeType
		{
			Cube, Sphere, WireCube, WireSphere, Line, Frustum, 
			Cone, Disc, WireDisc, Arc, WireArc, Rectangle
		};

		struct RawData
		{
			ShapeType shapeType;
			MeshType meshType;
			UINT32 idx;
			float distance;
			UINT32 numVertices;
			UINT32 numIndices;
		};

		/************************************************************************/
		/* 			Sort everything according to specified sorting rule         */
		/************************************************************************/

		UINT32 totalNumShapes = (UINT32)(mSolidCubeData.size() + mSolidSphereData.size() + 
			mWireCubeData.size() + mWireSphereData.size() + mLineData.size() + mFrustumData.size() + mConeData.size() +
			mDiscData.size() + mWireDiscData.size() + mArcData.size() + mWireArcData.size() + mRect3Data.size());

		UINT32 idx = 0;
		Vector<RawData> allShapes(totalNumShapes);

		UINT32 localIdx = 0;
		for (auto& shapeData : mSolidCubeData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Cube;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 24;
			rawData.numIndices = 36;
			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mSolidSphereData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Sphere;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsSphere(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mConeData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Cone;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsCone(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mDiscData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Disc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsDisc(shapeData.quality,
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mArcData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Arc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsArc(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mRect3Data)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Rectangle;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 4;
			rawData.numIndices = 6;
			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mWireCubeData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::WireCube;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 8;
			rawData.numIndices = 24;
			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mWireSphereData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::WireSphere;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireSphere(shapeData.quality,
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mLineData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::Line;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 2;
			rawData.numIndices = 2;
			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mFrustumData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::Frustum;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 8;
			rawData.numIndices = 36;
			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mWireDiscData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::WireDisc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireDisc(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		localIdx = 0;
		for (auto& shapeData : mWireArcData)
		{
			RawData& rawData = allShapes[idx];

			rawData.idx = localIdx++;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::WireArc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireArc(shapeData.quality,
				rawData.numVertices, rawData.numIndices);

			idx++;
		}

		if (sorting == SortType::FrontToBack)
		{
			std::sort(begin(allShapes), end(allShapes),
				[&](const RawData& x, const RawData& y)
			{
				return x.distance < y.distance;
			});
		}
		else if (sorting == SortType::BackToFront)
		{
			std::sort(begin(allShapes), end(allShapes),
				[&](const RawData& x, const RawData& y)
			{
				return y.distance < x.distance;
			});
		}

		/************************************************************************/
		/* 							Create batches                      		*/
		/************************************************************************/
		struct Batch
		{
			MeshType type;
			UINT32 startIdx;
			UINT32 endIdx;
			UINT32 numVertices;
			UINT32 numIndices;
		};

		Vector<Batch> batches;
		if (totalNumShapes > 0)
		{
			batches.push_back(Batch());

			{
				Batch& currentBatch = batches.back();
				currentBatch.startIdx = 0;
				currentBatch.type = allShapes[0].meshType;
				currentBatch.numVertices = allShapes[0].numVertices;
				currentBatch.numIndices = allShapes[0].numIndices;
			}

			for (UINT32 i = 1; i < totalNumShapes; i++)
			{
				Batch& currentBatch = batches.back();

				if (allShapes[i].meshType != currentBatch.type)
				{
					currentBatch.endIdx = i - 1;

					batches.push_back(Batch());

					Batch& newBatch = batches.back();
					newBatch.startIdx = i;
					newBatch.type = allShapes[i].meshType;
					newBatch.numVertices = allShapes[i].numVertices;
					newBatch.numIndices = allShapes[i].numIndices;
				}
				else
				{
					currentBatch.endIdx = i;
					currentBatch.numVertices += allShapes[i].numVertices;
					currentBatch.numIndices += allShapes[i].numIndices;
				}
			}

			{
				Batch& currentBatch = batches.back();
				currentBatch.endIdx = totalNumShapes - 1;
			}
		}

		/************************************************************************/
		/* 					Generate geometry for each batch                    */
		/************************************************************************/
		for (auto& batch : batches)
		{
			if (batch.type == MeshType::Solid)
			{
				MeshDataPtr meshData = bs_shared_ptr<MeshData>(batch.numVertices, batch.numIndices, mSolidVertexDesc);

				UINT32 curVertexOffset = 0;
				UINT32 curIndexOffet = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto normalIter = meshData->getVec3DataIter(VES_NORMAL);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];

					Matrix4* transform = nullptr;
					RGBA color = 0;

					switch (shapeData.shapeType)
					{
					case ShapeType::Cube:
					{
						CubeData& cubeData = mSolidCubeData[shapeData.idx];
						AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
						ShapeMeshes3D::solidAABox(box, meshData, curVertexOffset, curIndexOffet);

						transform = &cubeData.transform;
						color = cubeData.color.getAsRGBA();
					}
						break;
					case ShapeType::Sphere:
					{
						SphereData& sphereData = mSolidSphereData[shapeData.idx];
						Sphere sphere(sphereData.position, sphereData.radius);
						ShapeMeshes3D::solidSphere(sphere, meshData, curVertexOffset, curIndexOffet, sphereData.quality);

						transform = &sphereData.transform;
						color = sphereData.color.getAsRGBA();
					}
						break;
					case ShapeType::Cone:
					{
						ConeData& coneData = mConeData[shapeData.idx];
						ShapeMeshes3D::solidCone(coneData.base, coneData.normal, coneData.height, coneData.radius,
							meshData, curVertexOffset, curIndexOffet, coneData.quality);

						transform = &coneData.transform;
						color = coneData.color.getAsRGBA();
					}
						break;
					case ShapeType::Disc:
					{
						DiscData& discData = mDiscData[shapeData.idx];
						ShapeMeshes3D::solidDisc(discData.position, discData.radius, discData.normal,
							meshData, curVertexOffset, curIndexOffet, discData.quality);

						transform = &discData.transform;
						color = discData.color.getAsRGBA();
					}
						break;
					case ShapeType::Arc:
					{
						ArcData& arcData = mArcData[shapeData.idx];
						ShapeMeshes3D::solidArc(arcData.position, arcData.radius, arcData.normal,
							arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

						transform = &arcData.transform;
						color = arcData.color.getAsRGBA();
					}
						break;
					case ShapeType::Rectangle:
					{
						Rect3Data rectData = mRect3Data[shapeData.idx];
						ShapeMeshes3D::solidQuad(rectData.area, meshData, curVertexOffset, curIndexOffet);

						transform = &rectData.transform;
						color = rectData.color.getAsRGBA();
					}
						break;
					}

					Matrix4 transformIT = transform->inverseAffine().transpose();
					for (UINT32 i = 0; i < shapeData.numVertices; i++)
					{
						Vector3 worldPos = transform->multiplyAffine(positionIter.getValue());
						Vector3 worldNormal = transformIT.multiplyAffine(normalIter.getValue());

						positionIter.addValue(worldPos);
						normalIter.addValue(worldNormal);
						colorIter.addValue(color);
					}

					curVertexOffset += shapeData.numVertices;
					curIndexOffet += shapeData.numIndices;
				}

				mMeshes.push_back(ShapeMeshData());
				ShapeMeshData& newMesh = mMeshes.back();
				newMesh.mesh = mSolidMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
				newMesh.type = MeshType::Solid;
			}
			else // Wire
			{
				MeshDataPtr meshData = bs_shared_ptr<MeshData>(batch.numVertices,
					batch.numIndices, mWireVertexDesc);

				UINT32 curVertexOffset = 0;
				UINT32 curIndexOffet = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];

					Matrix4* transform = nullptr;
					RGBA color = 0;

					switch (shapeData.shapeType)
					{
					case ShapeType::WireCube:
					{
						CubeData& cubeData = mWireCubeData[shapeData.idx];

						AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
						ShapeMeshes3D::wireAABox(box, meshData, curVertexOffset, curIndexOffet);

						transform = &cubeData.transform;
						color = cubeData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireSphere:
					{
						SphereData& sphereData = mWireSphereData[shapeData.idx];

						Sphere sphere(sphereData.position, sphereData.radius);
						ShapeMeshes3D::wireSphere(sphere, meshData, curVertexOffset, curIndexOffet, sphereData.quality);

						transform = &sphereData.transform;
						color = sphereData.color.getAsRGBA();
					}
						break;
					case ShapeType::Line:
					{
						LineData& lineData = mLineData[shapeData.idx];

						ShapeMeshes3D::pixelLine(lineData.start, lineData.end, meshData, curVertexOffset, curIndexOffet);

						transform = &lineData.transform;
						color = lineData.color.getAsRGBA();
					}
						break;
					case ShapeType::Frustum:
					{
						FrustumData& frustumData = mFrustumData[shapeData.idx];

						ShapeMeshes3D::wireFrustum(frustumData.position, frustumData.aspect, frustumData.FOV, frustumData.near,
							frustumData.far, meshData, curVertexOffset, curIndexOffet);

						transform = &frustumData.transform;
						color = frustumData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireDisc:
					{
						DiscData& discData = mWireDiscData[shapeData.idx];

						ShapeMeshes3D::wireDisc(discData.position, discData.radius, discData.normal,
							meshData, curVertexOffset, curIndexOffet, discData.quality);

						transform = &discData.transform;
						color = discData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireArc:
					{
						ArcData& arcData = mWireArcData[shapeData.idx];

						ShapeMeshes3D::wireArc(arcData.position, arcData.radius, arcData.normal,
							arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

						transform = &arcData.transform;
						color = arcData.color.getAsRGBA();
					}
						break;
					}

					for (UINT32 i = 0; i < shapeData.numVertices; i++)
					{
						Vector3 worldPos = transform->multiplyAffine(positionIter.getValue());

						positionIter.addValue(worldPos);
						colorIter.addValue(color);
					}

					curVertexOffset += shapeData.numVertices;
					curIndexOffet += shapeData.numIndices;
				}

				mMeshes.push_back(ShapeMeshData());
				ShapeMeshData& newMesh = mMeshes.back();
				newMesh.mesh = mWireMeshHeap->alloc(meshData, DOT_LINE_LIST);
				newMesh.type = MeshType::Wire;
			}
		}
	}

	void DrawHelper::clearMeshes()
	{
		for (auto meshData : mMeshes)
		{
			if (meshData.type == MeshType::Solid)
				mSolidMeshHeap->dealloc(meshData.mesh);
			else
				mWireMeshHeap->dealloc(meshData.mesh);
		}

		mMeshes.clear();
	}
}