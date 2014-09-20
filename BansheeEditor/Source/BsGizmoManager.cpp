#include "BsGizmoManager.h"
#include "BsMesh.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsVertexDataDesc.h"
#include "BsDrawHelper3D.h"
#include "BsMeshHeap.h"
#include "BsCamera.h"
#include "BsSpriteTexture.h"
#include "BsCoreThread.h"
#include "BsBuiltinEditorResources.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsRenderSystem.h"
#include "BsRenderer.h"
#include "BsTransientMesh.h"

namespace BansheeEngine
{
	const UINT32 GizmoManager::VERTEX_BUFFER_GROWTH = 4096;
	const UINT32 GizmoManager::INDEX_BUFFER_GROWTH = 4096 * 2;
	const UINT32 GizmoManager::SPHERE_QUALITY = 1;
	const float GizmoManager::MAX_ICON_RANGE = 500.0f;
	const UINT32 GizmoManager::OPTIMAL_ICON_SIZE = 64;
	const float GizmoManager::ICON_TEXEL_WORLD_SIZE = 0.05f;

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
		mIconVertexDesc->addVertElem(VET_COLOR, VES_COLOR, 0);
		mIconVertexDesc->addVertElem(VET_COLOR, VES_COLOR, 1);

		mSolidMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mSolidVertexDesc);
		mWireMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mWireVertexDesc);
		mIconMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mIconVertexDesc);

		mSolidMaterial.material = BuiltinEditorResources::instance().createSolidGizmoMat();
		mWireMaterial.material = BuiltinEditorResources::instance().createWireGizmoMat();
		mIconMaterial.material = BuiltinEditorResources::instance().createIconGizmoMat();

		mSolidMaterial.proxy = mSolidMaterial.material->_createProxy();
		mWireMaterial.proxy = mWireMaterial.material->_createProxy();
		mIconMaterial.proxy = mIconMaterial.material->_createProxy();

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::initializeCore, this));
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

	void GizmoManager::initializeCore()
	{
		THROW_IF_NOT_CORE_THREAD;

		// TODO - Make a better interface when dealing with parameters through proxies?
		{
			MaterialProxyPtr proxy = mWireMaterial.proxy;
			GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mWireMaterial.mViewProj);
		}

		{
			MaterialProxyPtr proxy = mSolidMaterial.proxy;
			GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mSolidMaterial.mViewProj);
			vertParams->getParam("matViewIT", mSolidMaterial.mViewIT);
		}

		{
			MaterialProxyPtr proxy = mIconMaterial.proxy;
			GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mIconMaterial.mViewProj);

			GpuParamsPtr fragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];

			fragParams->getTextureParam("mainTexture", mIconMaterial.mTexture);
		}
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
		IconRenderDataVecPtr iconRenderData = buildIconMesh();

		// TODO - This must be rendered while Scene view is being rendered
		Matrix4 viewMat = mCamera->getViewMatrix();
		Matrix4 projMat = mCamera->getProjectionMatrix();
		ViewportPtr viewport = mCamera->getViewport();

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::coreRenderSolidGizmos, 
			this, viewMat, projMat, mSolidMesh->_createProxy(0)));

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::coreRenderWireGizmos, 
			this, viewMat, projMat, mWireMesh->_createProxy(0)));

		RectI screenArea = mCamera->getViewport()->getArea();

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::coreRenderIconGizmos, 
			this, screenArea, mIconMesh->_createProxy(0), iconRenderData));
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

	GizmoManager::IconRenderDataVecPtr GizmoManager::buildIconMesh()
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
				HSpriteTexture texA = mIconData[a.iconIdx].texture->getTexture();
				HSpriteTexture texB = mIconData[b.iconIdx].texture->getTexture();

				if (texA == texB)
					return a.iconIdx < b.iconIdx;

				return texA->getInternalID() < texB->getInternalID();
			}
			else
				return a.distance > b.distance;
		});

		MeshDataPtr meshData = bs_shared_ptr<MeshData>(actualNumIcons * 4, actualNumIcons * 6, mIconVertexDesc);

		auto positionIter = meshData->getVec3DataIter(VES_POSITION);
		auto texcoordIter = meshData->getVec2DataIter(VES_TEXCOORD);
		auto normalColorIter = meshData->getDWORDDataIter(VES_COLOR, 0);
		auto fadedColorIter = meshData->getDWORDDataIter(VES_COLOR, 1);

		UINT32* indices = meshData->getIndices32();

		float cameraScale = 1.0f;
		if (mCamera->getProjectionType() == PT_ORTHOGRAPHIC)
			cameraScale = mCamera->getViewport()->getHeight() / mCamera->getOrthoWindowHeight();
		else
		{
			Radian vertFOV(Math::tan(mCamera->getHorzFOV() * 0.5f));
			cameraScale = (mCamera->getViewport()->getHeight() * 0.5f) / vertFOV.valueRadians();
		}

		IconRenderDataVecPtr iconRenderData = bs_shared_ptr<IconRenderDataVec>();
		UINT32 lastTextureIdx = 0;
		HTexture curTexture;

		// Note: This assumes the meshes will be rendered using the same camera
		// properties as when they are created
		for (i = 0; i < actualNumIcons; i++)
		{
			SortedIconData& sortedIconData = mSortedIconData[i];
			IconData& iconData = mIconData[sortedIconData.iconIdx];

			if (curTexture != iconData.texture)
			{
				UINT32 numIconsPerTexture = i - lastTextureIdx;
				if (numIconsPerTexture > 0)
				{
					iconRenderData->push_back(IconRenderData());
					IconRenderData& renderData = iconRenderData->back();
					renderData.count = numIconsPerTexture;
					renderData.texture = curTexture;
				}

				lastTextureIdx = i;
				curTexture = iconData.texture;
			}

			UINT32 iconWidth = iconData.texture->getWidth();
			UINT32 iconHeight = iconData.texture->getHeight();

			limitIconSize(iconWidth, iconHeight);

			Color normalColor, fadedColor;
			calculateIconColors(iconData.color, *mCamera.get(), iconHeight, iconData.fixedScale, normalColor, fadedColor);

			Vector3 position(sortedIconData.screenPosition.x, sortedIconData.screenPosition.y, sortedIconData.distance);
			// TODO - Does the depth need to be corrected since it was taken from a projective camera (probably)?

			float halfWidth = iconWidth * 0.5f;
			float halfHeight = iconHeight * 0.5f;

			if (!iconData.fixedScale)
			{
				float iconScale = 1.0f;
				if (mCamera->getProjectionType() == PT_ORTHOGRAPHIC)
					iconScale = cameraScale;
				else
					iconScale = cameraScale / sortedIconData.distance;

				halfWidth *= iconScale;
				halfHeight *= iconScale;
			}

			Vector3 positions[4];
			positions[0] = position + Vector3(-halfWidth, -halfHeight, 0.0f);
			positions[1] = position + Vector3(halfWidth, -halfHeight, 0.0f);
			positions[2] = position + Vector3(-halfWidth, halfHeight, 0.0f);
			positions[3] = position + Vector3(halfWidth, halfHeight, 0.0f);

			Vector2 uvs[4];
			uvs[0] = iconData.texture->transformUV(Vector2(0.0f, 0.0f));
			uvs[1] = iconData.texture->transformUV(Vector2(1.0f, 0.0f));
			uvs[2] = iconData.texture->transformUV(Vector2(0.0f, 1.0f));
			uvs[3] = iconData.texture->transformUV(Vector2(1.0f, 1.0f));

			for (UINT32 j = 0; j < 4; j++)
			{
				positionIter.addValue(positions[j]);
				texcoordIter.addValue(uvs[j]);
				normalColorIter.addValue(normalColor.getAsRGBA());
				fadedColorIter.addValue(fadedColor.getAsRGBA());
			}

			UINT32 vertOffset = i * 4;

			indices[0] = vertOffset + 0;
			indices[1] = vertOffset + 1;
			indices[2] = vertOffset + 2;
			indices[3] = vertOffset + 1;
			indices[4] = vertOffset + 3;
			indices[5] = vertOffset + 2;

			indices += 6;
		}

		if (mIconMesh != nullptr)
			mIconMeshHeap->dealloc(mIconMesh);

		mIconMesh = mIconMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);

		return iconRenderData;
	}

	void GizmoManager::limitIconSize(UINT32& width, UINT32& height)
	{
		if (width <= OPTIMAL_ICON_SIZE && height <= OPTIMAL_ICON_SIZE)
			return;

		float relWidth = OPTIMAL_ICON_SIZE / width;
		float relHeight = OPTIMAL_ICON_SIZE / height;

		float scale = std::min(relWidth, relHeight);

		width = Math::roundToInt(width * scale);
		height = Math::roundToInt(height * scale);
	}

	void GizmoManager::calculateIconColors(const Color& tint, const Camera& camera,
		UINT32 iconHeight, bool fixedScale, Color& normalColor, Color& fadedColor)
	{
		normalColor = tint;

		if (!fixedScale)
		{
			float iconToScreenRatio = iconHeight / (float)camera.getViewport()->getHeight();

			if (iconToScreenRatio > 0.3f)
			{
				float alpha = 1.0f - Math::lerp01(iconToScreenRatio, 0.3f, 1.0f);
				normalColor.a *= alpha;
			}
			else if (iconToScreenRatio < 0.1f)
			{
				float alpha = Math::lerp01(iconToScreenRatio, 0.0f, 0.1f);
				normalColor.a *= alpha;
			}
		}

		fadedColor = normalColor;
		fadedColor.a *= 0.2f;
	}

	void GizmoManager::coreRenderSolidGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;
		Matrix4 viewIT = viewMatrix.inverse().transpose();

		mSolidMaterial.mViewProj.set(viewProjMat);
		mSolidMaterial.mViewIT.set(viewIT);

		Renderer::setPass(*mSolidMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}

	void GizmoManager::coreRenderWireGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;
		Matrix4 viewIT = viewMatrix.inverse().transpose();

		mWireMaterial.mViewProj.set(viewProjMat);

		Renderer::setPass(*mWireMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}

	void GizmoManager::coreRenderIconGizmos(RectI screenArea, MeshProxyPtr meshProxy, IconRenderDataVecPtr renderData)
	{
		RenderSystem& rs = RenderSystem::instance();
		MeshBasePtr mesh;

		// TODO: Instead of this lock consider just storing all needed data in MeshProxy and not referencing Mesh at all?
		if (!meshProxy->mesh.expired())
			mesh = meshProxy->mesh.lock();
		else
			return;

		std::shared_ptr<VertexData> vertexData = mesh->_getVertexData();

		rs.setVertexDeclaration(vertexData->vertexDeclaration);
		auto vertexBuffers = vertexData->getBuffers();

		VertexBufferPtr vertBuffers[1] = { vertexBuffers.begin()->second };
		rs.setVertexBuffers(0, vertBuffers, 1);

		IndexBufferPtr indexBuffer = mesh->_getIndexBuffer();
		rs.setIndexBuffer(indexBuffer);

		rs.setDrawOperation(DOT_TRIANGLE_LIST);

		// Set up ortho matrix
		Matrix4 projMat; 

		float left = screenArea.x + rs.getHorizontalTexelOffset();
		float right = screenArea.x + screenArea.width + rs.getHorizontalTexelOffset();
		float top = screenArea.y + rs.getVerticalTexelOffset();
		float bottom = screenArea.y + screenArea.height + rs.getVerticalTexelOffset();
		float near = rs.getMinimumDepthInputValue();
		float far = rs.getMaximumDepthInputValue();

		projMat.makeProjectionOrtho(left, right, top, bottom, near, far);
		mIconMaterial.mViewProj.set(projMat);

		for (UINT32 passIdx = 0; passIdx < 2; passIdx++)
		{
			Renderer::setPass(*mIconMaterial.proxy, passIdx);

			UINT32 curIndexOffset = 0;
			for (auto curRenderData : *renderData)
			{
				mIconMaterial.mTexture.set(curRenderData.texture);

				rs.drawIndexed(curIndexOffset, curRenderData.count * 6, 0, curRenderData.count * 4);
				curIndexOffset += curRenderData.count * 6;

			}
		}

		mesh->_notifyUsedOnGPU();
	}
}