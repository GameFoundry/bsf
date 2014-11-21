#include "BsGizmoManager.h"
#include "BsMesh.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsVertexDataDesc.h"
#include "BsShapeMeshes3D.h"
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
#include "BsRendererManager.h"
#include "BsDrawHelper.h"
#include "BsSceneEditorWidget.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GizmoManager::VERTEX_BUFFER_GROWTH = 4096;
	const UINT32 GizmoManager::INDEX_BUFFER_GROWTH = 4096 * 2;
	const UINT32 GizmoManager::SPHERE_QUALITY = 1;
	const UINT32 GizmoManager::WIRE_SPHERE_QUALITY = 10;
	const float GizmoManager::MAX_ICON_RANGE = 500.0f;
	const UINT32 GizmoManager::OPTIMAL_ICON_SIZE = 64;
	const float GizmoManager::ICON_TEXEL_WORLD_SIZE = 0.05f;

	GizmoManager::GizmoManager()
		:mPickable(false), mDrawHelper(nullptr), mPickingDrawHelper(nullptr), mCore(nullptr), mCurrentIdx(0)
	{
		mTransform = Matrix4::IDENTITY;
		mDrawHelper = bs_new<DrawHelper>();
		mPickingDrawHelper = bs_new<DrawHelper>();

		mIconVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mIconVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mIconVertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD);
		mIconVertexDesc->addVertElem(VET_COLOR, VES_COLOR, 0);
		mIconVertexDesc->addVertElem(VET_COLOR, VES_COLOR, 1);

		mIconMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mIconVertexDesc);

		HMaterial solidMaterial = BuiltinEditorResources::instance().createSolidGizmoMat();
		HMaterial wireMaterial = BuiltinEditorResources::instance().createWireGizmoMat();
		HMaterial iconMaterial = BuiltinEditorResources::instance().createIconGizmoMat();
		HMaterial pickingMaterial = BuiltinEditorResources::instance().createGizmoPickingMat();
		HMaterial alphaPickingMaterial = BuiltinEditorResources::instance().createAlphaGizmoPickingMat();

		CoreInitData initData;

		initData.solidMatProxy = solidMaterial->_createProxy();
		initData.wireMatProxy = wireMaterial->_createProxy();
		initData.iconMatProxy = iconMaterial->_createProxy();
		initData.pickingMatProxy = pickingMaterial->_createProxy();
		initData.alphaPickingMatProxy = alphaPickingMaterial->_createProxy();

		mCore = bs_new<GizmoManagerCore>(GizmoManagerCore::PrivatelyConstuct());

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::initializeCore, this, initData));
	}

	GizmoManager::~GizmoManager()
	{
		if (mIconMesh != nullptr)
			mIconMeshHeap->dealloc(mIconMesh);

		bs_delete(mDrawHelper);
		bs_delete(mPickingDrawHelper);

		gCoreAccessor().queueCommand(std::bind(&GizmoManager::destroyCore, this, mCore));
	}

	void GizmoManager::initializeCore(const CoreInitData& initData)
	{
		mCore->initialize(initData);
	}

	void GizmoManager::destroyCore(GizmoManagerCore* core)
	{
		bs_delete(core);
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
		mDrawHelper->setColor(color);
		mColor = color;
	}

	void GizmoManager::setTransform(const Matrix4& transform)
	{
		mDrawHelper->setTransform(transform);
		mTransform = transform;
	}

	void GizmoManager::drawCube(const Vector3& position, const Vector3& extents)
	{
		mSolidCubeData.push_back(CubeData());
		CubeData& cubeData = mSolidCubeData.back();

		cubeData.idx = mCurrentIdx++;
		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.sceneObject = mActiveSO;
		cubeData.pickable = mPickable;

		mDrawHelper->cube(position, extents);
		mIdxToSceneObjectMap[cubeData.idx] = mActiveSO;
	}

	void GizmoManager::drawSphere(const Vector3& position, float radius)
	{
		mSolidSphereData.push_back(SphereData());
		SphereData& sphereData = mSolidSphereData.back();

		sphereData.idx = mCurrentIdx++;
		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.sceneObject = mActiveSO;
		sphereData.pickable = mPickable;

		mDrawHelper->sphere(position, radius);
		mIdxToSceneObjectMap[sphereData.idx] = mActiveSO;
	}

	void GizmoManager::drawWireCube(const Vector3& position, const Vector3& extents)
	{
		mWireCubeData.push_back(CubeData());
		CubeData& cubeData = mWireCubeData.back();

		cubeData.idx = mCurrentIdx++;
		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.sceneObject = mActiveSO;
		cubeData.pickable = mPickable;

		mDrawHelper->wireCube(position, extents);
		mIdxToSceneObjectMap[cubeData.idx] = mActiveSO;
	}

	void GizmoManager::drawWireSphere(const Vector3& position, float radius)
	{
		mWireSphereData.push_back(SphereData());
		SphereData& sphereData = mWireSphereData.back();

		sphereData.idx = mCurrentIdx++;
		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.sceneObject = mActiveSO;
		sphereData.pickable = mPickable;

		mDrawHelper->wireSphere(position, radius);
		mIdxToSceneObjectMap[sphereData.idx] = mActiveSO;
	}

	void GizmoManager::drawLine(const Vector3& start, const Vector3& end)
	{
		mLineData.push_back(LineData());
		LineData& lineData = mLineData.back();

		lineData.idx = mCurrentIdx++;
		lineData.start = start;
		lineData.end = end;
		lineData.color = mColor;
		lineData.transform = mTransform;
		lineData.sceneObject = mActiveSO;
		lineData.pickable = mPickable;

		mDrawHelper->line(start, end);
		mIdxToSceneObjectMap[lineData.idx] = mActiveSO;
	}

	void GizmoManager::drawFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far)
	{
		mFrustumData.push_back(FrustumData());
		FrustumData& frustumData = mFrustumData.back();

		frustumData.idx = mCurrentIdx++;
		frustumData.position = position;
		frustumData.aspect = aspect;
		frustumData.FOV = FOV;
		frustumData.near = near;
		frustumData.far = far;
		frustumData.color = mColor;
		frustumData.transform = mTransform;
		frustumData.sceneObject = mActiveSO;
		frustumData.pickable = mPickable;

		mDrawHelper->frustum(position, aspect, FOV, near, far);
		mIdxToSceneObjectMap[frustumData.idx] = mActiveSO;
	}

	void GizmoManager::drawIcon(Vector3 position, HSpriteTexture image, bool fixedScale)
	{
		mIconData.push_back(IconData());
		IconData& iconData = mIconData.back();

		iconData.idx = mCurrentIdx++;
		iconData.position = position;
		iconData.texture = image;
		iconData.fixedScale = fixedScale;
		iconData.color = mColor;
		iconData.transform = mTransform;
		iconData.sceneObject = mActiveSO;
		iconData.pickable = mPickable;

		mIdxToSceneObjectMap[iconData.idx] = mActiveSO;
	}

	void GizmoManager::update(const CameraHandlerPtr& camera)
	{
		mDrawHelper->clearMeshes();

		if (mIconMesh != nullptr)
			mIconMeshHeap->dealloc(mIconMesh);

		RenderTargetPtr rt;
		SceneEditorWidget* sceneView = SceneViewLocator::instance();
		if (sceneView != nullptr)
		{
			rt = camera->getViewport()->getTarget();

			IconRenderDataVecPtr iconRenderData;

			mDrawHelper->buildMeshes();
			const Vector<DrawHelper::ShapeMeshData>& meshes = mDrawHelper->getMeshes();

			SPtr<MeshCoreBase> solidMesh = nullptr;
			SPtr<MeshCoreBase> wireMesh = nullptr;
			for (auto& meshData : meshes)
			{
				if (meshData.type == DrawHelper::MeshType::Solid)
				{
					if (solidMesh == nullptr)
						solidMesh = meshData.mesh->getCore();
				}
				else // Wire
				{
					if (wireMesh == nullptr)
						wireMesh = meshData.mesh->getCore();
				}
			}

			// Since there is no sorting used with draw helper meshes we only expect up to two of them,
			// one for solids, one for wireframe
			assert(meshes.size() <= 2);

			mIconMesh = buildIconMesh(camera, mIconData, false, iconRenderData);
			SPtr<MeshCoreBase> iconMesh = mIconMesh->getCore();

			gCoreAccessor().queueCommand(std::bind(&GizmoManagerCore::updateData, mCore, rt, solidMesh, wireMesh, iconMesh, iconRenderData));
		}
		else
		{
			mIconMesh = nullptr;

			IconRenderDataVecPtr iconRenderData = bs_shared_ptr<IconRenderDataVec>();
			gCoreAccessor().queueCommand(std::bind(&GizmoManagerCore::updateData, mCore, nullptr, nullptr, nullptr, nullptr, iconRenderData));
		}
	}

	void GizmoManager::renderForPicking(const CameraHandlerPtr& camera, std::function<Color(UINT32)> idxToColorCallback)
	{
		Vector<IconData> iconData;
		IconRenderDataVecPtr iconRenderData;

		mPickingDrawHelper->clear();

		for (auto& cubeDataEntry : mSolidCubeData)
		{
			if (!cubeDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(cubeDataEntry.idx));
			mPickingDrawHelper->setTransform(cubeDataEntry.transform);

			mPickingDrawHelper->cube(cubeDataEntry.position, cubeDataEntry.extents);
		}

		for (auto& cubeDataEntry : mWireCubeData)
		{
			if (!cubeDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(cubeDataEntry.idx));
			mPickingDrawHelper->setTransform(cubeDataEntry.transform);

			mPickingDrawHelper->wireCube(cubeDataEntry.position, cubeDataEntry.extents);
		}

		for (auto& sphereDataEntry : mSolidSphereData)
		{
			if (!sphereDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(sphereDataEntry.idx));
			mPickingDrawHelper->setTransform(sphereDataEntry.transform);

			mPickingDrawHelper->sphere(sphereDataEntry.position, sphereDataEntry.radius);
		}

		for (auto& sphereDataEntry : mWireSphereData)
		{
			if (!sphereDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(sphereDataEntry.idx));
			mPickingDrawHelper->setTransform(sphereDataEntry.transform);

			mPickingDrawHelper->wireSphere(sphereDataEntry.position, sphereDataEntry.radius);
		}

		for (auto& lineDataEntry : mLineData)
		{
			if (!lineDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(lineDataEntry.idx));
			mPickingDrawHelper->setTransform(lineDataEntry.transform);

			mPickingDrawHelper->line(lineDataEntry.start, lineDataEntry.end);
		}

		for (auto& frustumDataEntry : mFrustumData)
		{
			if (!frustumDataEntry.pickable)
				continue;

			mPickingDrawHelper->setColor(idxToColorCallback(frustumDataEntry.idx));
			mPickingDrawHelper->setTransform(frustumDataEntry.transform);

			mPickingDrawHelper->frustum(frustumDataEntry.position, frustumDataEntry.aspect, frustumDataEntry.FOV, 
				frustumDataEntry.near, frustumDataEntry.far);
		}

		for (auto& iconDataEntry : mIconData)
		{
			if (!iconDataEntry.pickable)
				continue;

			iconData.push_back(iconDataEntry);
			iconData.back().color = idxToColorCallback(iconDataEntry.idx);
		}

		mPickingDrawHelper->buildMeshes();
		const Vector<DrawHelper::ShapeMeshData>& meshes = mPickingDrawHelper->getMeshes();

		TransientMeshPtr iconMesh = buildIconMesh(camera, iconData, true, iconRenderData);

		// Note: This must be rendered while Scene view is being rendered
		Matrix4 viewMat = camera->getViewMatrix();
		Matrix4 projMat = camera->getProjectionMatrixRS();
		ViewportPtr viewport = camera->getViewport();

		for (auto& meshData : meshes)
		{
			if (meshData.type == DrawHelper::MeshType::Solid)
			{
				gCoreAccessor().queueCommand(std::bind(&GizmoManagerCore::renderGizmos,
					mCore, viewMat, projMat, meshData.mesh->getCore(), GizmoMaterial::Picking));
			}
			else // Wire
			{
				gCoreAccessor().queueCommand(std::bind(&GizmoManagerCore::renderGizmos,
					mCore, viewMat, projMat, meshData.mesh->getCore(), GizmoMaterial::Picking));
			}
		}

		Rect2I screenArea = camera->getViewport()->getArea();

		gCoreAccessor().queueCommand(std::bind(&GizmoManagerCore::renderIconGizmos,
			mCore, screenArea, iconMesh->getCore(), iconRenderData, true));

		mPickingDrawHelper->clearMeshes();
		mIconMeshHeap->dealloc(iconMesh);
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
		mIdxToSceneObjectMap.clear();

		mDrawHelper->clear();

		mCurrentIdx = 0;
	}

	TransientMeshPtr GizmoManager::buildIconMesh(const CameraHandlerPtr& camera, const Vector<IconData>& iconData,
		bool forPicking, GizmoManager::IconRenderDataVecPtr& iconRenderData)
	{
		mSortedIconData.clear();
		
		if (iconData.size() > mSortedIconData.size())
			mSortedIconData.resize(iconData.size());

		UINT32 i = 0;
		for (auto& iconEntry : iconData)
		{
			Vector3 viewPoint = camera->worldToViewPoint(iconEntry.position);

			float distance = -viewPoint.z;
			if (distance < camera->getNearClipDistance()) // Ignore behind clip plane
				continue;

			if (distance > MAX_ICON_RANGE) // Ignore too far away
				continue;

			if (!iconEntry.texture) // Ignore missing texture
				continue;

			if (forPicking && !iconEntry.pickable)
				continue;

			SortedIconData& sortedIconData = mSortedIconData[i];
			sortedIconData.iconIdx = i;
			sortedIconData.distance = distance;
			sortedIconData.screenPosition = camera->viewToScreenPoint(viewPoint);

			i++;
		}

		UINT32 actualNumIcons = i;

		// Sort back to front first, then by texture
		std::sort(mSortedIconData.begin(), mSortedIconData.begin() + actualNumIcons, 
			[&](const SortedIconData& a, const SortedIconData& b)
		{
			if (a.distance == b.distance)
			{
				HTexture texA = iconData[a.iconIdx].texture->getTexture();
				HTexture texB = iconData[b.iconIdx].texture->getTexture();

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
		if (camera->getProjectionType() == PT_ORTHOGRAPHIC)
			cameraScale = camera->getViewport()->getHeight() / camera->getOrthoWindowHeight();
		else
		{
			Radian vertFOV(Math::tan(camera->getHorzFOV() * 0.5f));
			cameraScale = (camera->getViewport()->getHeight() * 0.5f) / vertFOV.valueRadians();
		}

		iconRenderData = bs_shared_ptr<IconRenderDataVec>();
		UINT32 lastTextureIdx = std::numeric_limits<UINT32>::max();
		HTexture curTexture;

		// Note: This assumes the meshes will be rendered using the same camera
		// properties as when they are created
		for (i = 0; i < actualNumIcons; i++)
		{
			SortedIconData& sortedIconData = mSortedIconData[i];
			const IconData& curIconData = iconData[sortedIconData.iconIdx];

			HTexture atlasTexture = curIconData.texture->getTexture();

			if (curTexture != atlasTexture)
			{
				UINT32 numIconsPerTexture = i - lastTextureIdx;
				if (numIconsPerTexture > 0)
				{
					iconRenderData->push_back(IconRenderData());
					IconRenderData& renderData = iconRenderData->back();
					renderData.count = numIconsPerTexture;
					renderData.texture = atlasTexture->getCore();
				}

				lastTextureIdx = i;
				curTexture = atlasTexture;
			}

			UINT32 iconWidth = curIconData.texture->getWidth();
			UINT32 iconHeight = curIconData.texture->getHeight();

			limitIconSize(iconWidth, iconHeight);

			Vector3 position((float)sortedIconData.screenPosition.x, (float)sortedIconData.screenPosition.y, -sortedIconData.distance);
			Vector3 projPosition = camera->projectPoint(position);
			position.z = projPosition.z;

			float halfWidth = iconWidth * 0.5f;
			float halfHeight = iconHeight * 0.5f;

			if (!curIconData.fixedScale)
			{
				float iconScale = 1.0f;
				if (camera->getProjectionType() == PT_ORTHOGRAPHIC)
					iconScale = cameraScale * ICON_TEXEL_WORLD_SIZE;
				else
					iconScale = (cameraScale * ICON_TEXEL_WORLD_SIZE) / sortedIconData.distance;

				halfWidth *= iconScale;
				halfHeight *= iconScale;
			}

			Color normalColor, fadedColor;
			calculateIconColors(curIconData.color, camera, (UINT32)(halfHeight * 2.0f), curIconData.fixedScale, normalColor, fadedColor);

			if (forPicking)
			{
				normalColor = curIconData.color;
				fadedColor = curIconData.color;
			}

			Vector3 positions[4];
			positions[0] = position + Vector3(-halfWidth, -halfHeight, 0.0f);
			positions[1] = position + Vector3(halfWidth, -halfHeight, 0.0f);
			positions[2] = position + Vector3(halfWidth, halfHeight, 0.0f);
			positions[3] = position + Vector3(-halfWidth, halfHeight, 0.0f);

			Vector2 uvs[4];
			uvs[0] = curIconData.texture->transformUV(Vector2(0.0f, 0.0f));
			uvs[1] = curIconData.texture->transformUV(Vector2(1.0f, 0.0f));
			uvs[2] = curIconData.texture->transformUV(Vector2(1.0f, 1.0f));
			uvs[3] = curIconData.texture->transformUV(Vector2(0.0f, 1.0f));

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
			indices[3] = vertOffset + 0;
			indices[4] = vertOffset + 2;
			indices[5] = vertOffset + 3;

			indices += 6;
		}

		return mIconMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
	}

	void GizmoManager::limitIconSize(UINT32& width, UINT32& height)
	{
		if (width <= OPTIMAL_ICON_SIZE && height <= OPTIMAL_ICON_SIZE)
			return;

		float relWidth = OPTIMAL_ICON_SIZE / (float)width;
		float relHeight = OPTIMAL_ICON_SIZE / (float)height;

		float scale = std::min(relWidth, relHeight);

		width = Math::roundToInt(width * scale);
		height = Math::roundToInt(height * scale);
	}

	void GizmoManager::calculateIconColors(const Color& tint, const CameraHandlerPtr& camera,
		UINT32 iconHeight, bool fixedScale, Color& normalColor, Color& fadedColor)
	{
		normalColor = tint;

		if (!fixedScale)
		{
			float iconToScreenRatio = iconHeight / (float)camera->getViewport()->getHeight();

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

	HSceneObject GizmoManager::getSceneObject(UINT32 gizmoIdx)
	{
		auto iterFind = mIdxToSceneObjectMap.find(gizmoIdx);

		if (iterFind != mIdxToSceneObjectMap.end())
			return iterFind->second;

		return HSceneObject();
	}

	const float GizmoManagerCore::PICKING_ALPHA_CUTOFF = 0.5f;

	GizmoManagerCore::GizmoManagerCore(const PrivatelyConstuct& dummy)
	{
	}

	void GizmoManagerCore::initialize(const GizmoManager::CoreInitData& initData)
	{
		THROW_IF_NOT_CORE_THREAD;

		mSolidMaterial.proxy = initData.solidMatProxy;
		mWireMaterial.proxy = initData.wireMatProxy;
		mIconMaterial.proxy = initData.iconMatProxy;
		mPickingMaterial.proxy = initData.pickingMatProxy;
		mAlphaPickingMaterial.proxy = initData.alphaPickingMatProxy;

		// TODO - Make a better interface when dealing with parameters through proxies?
		{
			MaterialProxyPtr proxy = mWireMaterial.proxy;
			SPtr<GpuParamsCore> vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mWireMaterial.mViewProj);
		}

		{
			MaterialProxyPtr proxy = mSolidMaterial.proxy;
			SPtr<GpuParamsCore> vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mSolidMaterial.mViewProj);
		}

		{
			MaterialProxyPtr proxy = mIconMaterial.proxy;
			SPtr<GpuParamsCore> vertParams0 = proxy->params[proxy->passes[0].vertexProgParamsIdx];
			SPtr<GpuParamsCore> vertParams1 = proxy->params[proxy->passes[1].vertexProgParamsIdx];

			vertParams0->getParam("matViewProj", mIconMaterial.mViewProj[0]);
			vertParams1->getParam("matViewProj", mIconMaterial.mViewProj[1]);

			mIconMaterial.mFragParams[0] = proxy->params[proxy->passes[0].fragmentProgParamsIdx];
			mIconMaterial.mFragParams[1] = proxy->params[proxy->passes[1].fragmentProgParamsIdx];

			mIconMaterial.mFragParams[0]->getTextureParam("mainTexture", mIconMaterial.mTexture[0]);
			mIconMaterial.mFragParams[1]->getTextureParam("mainTexture", mIconMaterial.mTexture[1]);
		}

		{
			MaterialProxyPtr proxy = mPickingMaterial.proxy;
			SPtr<GpuParamsCore> vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mPickingMaterial.mViewProj);
		}

		{
			MaterialProxyPtr proxy = mAlphaPickingMaterial.proxy;
			SPtr<GpuParamsCore> vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mAlphaPickingMaterial.mViewProj);

			mAlphaPickingMaterial.mFragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];
			mAlphaPickingMaterial.mFragParams->getTextureParam("mainTexture", mAlphaPickingMaterial.mTexture);

			GpuParamFloatCore alphaCutoffParam;
			mAlphaPickingMaterial.mFragParams->getParam("alphaCutoff", alphaCutoffParam);
			alphaCutoffParam.set(PICKING_ALPHA_CUTOFF);
		}

		RendererPtr activeRenderer = RendererManager::instance().getActive();
		activeRenderer->onCorePostRenderViewport.connect(std::bind(&GizmoManagerCore::render, this, _1));
	}

	void GizmoManagerCore::updateData(const RenderTargetPtr& rt, const SPtr<MeshCoreBase>& solidMesh, const SPtr<MeshCoreBase>& wireMesh,
		const SPtr<MeshCoreBase>& iconMesh, const GizmoManager::IconRenderDataVecPtr& iconRenderData)
	{
		mSceneRenderTarget = rt;
		mSolidMesh = solidMesh;
		mWireMesh = wireMesh;
		mIconMesh = iconMesh;
		mIconRenderData = iconRenderData;
	}

	void GizmoManagerCore::render(const CameraHandlerCore& camera)
	{
		SPtr<RenderTargetCore> sceneRenderTarget = mSceneRenderTarget->getCore();

		if (camera.getViewport()->getTarget() != sceneRenderTarget)
			return;

		float width = (float)sceneRenderTarget->getProperties().getWidth();
		float height = (float)sceneRenderTarget->getProperties().getHeight();

		Rect2 normArea = camera.getViewport()->getNormArea();

		Rect2I screenArea;
		screenArea.x = (int)(normArea.x * width);
		screenArea.y = (int)(normArea.y * height);
		screenArea.width = (int)(normArea.width * width);
		screenArea.height = (int)(normArea.height * height);

		if (mSolidMesh != nullptr)
			renderGizmos(camera.getViewMatrix(), camera.getProjectionMatrixRS(), mSolidMesh, GizmoManager::GizmoMaterial::Solid);

		if (mWireMesh != nullptr)
			renderGizmos(camera.getViewMatrix(), camera.getProjectionMatrixRS(), mWireMesh, GizmoManager::GizmoMaterial::Wire);

		if (mIconMesh != nullptr)
			renderIconGizmos(screenArea, mIconMesh, mIconRenderData, false);
	}

	void GizmoManagerCore::renderGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, SPtr<MeshCoreBase> mesh, GizmoManager::GizmoMaterial material)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;

		switch (material)
		{
		case GizmoManager::GizmoMaterial::Solid:
			mSolidMaterial.mViewProj.set(viewProjMat);
			Renderer::setPass(*mSolidMaterial.proxy, 0);
			break;
		case GizmoManager::GizmoMaterial::Wire:
			mWireMaterial.mViewProj.set(viewProjMat);
			Renderer::setPass(*mWireMaterial.proxy, 0);
			break;
		case GizmoManager::GizmoMaterial::Picking:
			mPickingMaterial.mViewProj.set(viewProjMat);
			Renderer::setPass(*mPickingMaterial.proxy, 0);
			break;
		}
		
		Renderer::draw(mesh, mesh->getProperties().getSubMesh(0));
	}

	void GizmoManagerCore::renderIconGizmos(Rect2I screenArea, SPtr<MeshCoreBase> mesh, GizmoManager::IconRenderDataVecPtr renderData, bool usePickingMaterial)
	{
		RenderSystem& rs = RenderSystem::instance();
		const MeshProperties& meshProps = mesh->getProperties();

		std::shared_ptr<VertexData> vertexData = mesh->getVertexData();

		rs.setVertexDeclaration(vertexData->vertexDeclaration);
		auto vertexBuffers = vertexData->getBuffers();

		SPtr<VertexBufferCore> vertBuffers[1] = { vertexBuffers.begin()->second };
		rs.setVertexBuffers(0, vertBuffers, 1);

		SPtr<IndexBufferCore> indexBuffer = mesh->getIndexBuffer();
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

		// Top/bottom have been swapped because we're moving from window coordinates (origin top left)
		// to normalized device coordinates (origin bottom left)
		// Negative near/far because Z is flipped for normalized device coordinates 
		// (positive Z goes into screen as opposed to view space here we're looking along negative Z)
		projMat.makeProjectionOrtho(left, right, top, bottom, -near, -far);

		if (!usePickingMaterial)
		{
			mIconMaterial.mViewProj[0].set(projMat);
			mIconMaterial.mViewProj[1].set(projMat);

			for (UINT32 passIdx = 0; passIdx < 2; passIdx++)
			{
				Renderer::setPass(*mIconMaterial.proxy, passIdx);

				UINT32 curIndexOffset = mesh->getIndexOffset();
				for (auto curRenderData : *renderData)
				{
					mIconMaterial.mTexture[passIdx].set(curRenderData.texture);
					rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, mIconMaterial.mFragParams[passIdx]);

					rs.drawIndexed(curIndexOffset, curRenderData.count * 6, mesh->getVertexOffset(), curRenderData.count * 4);
					curIndexOffset += curRenderData.count * 6;
				}
			}
		}
		else
		{
			mAlphaPickingMaterial.mViewProj.set(projMat);

			Renderer::setPass(*mAlphaPickingMaterial.proxy, 0);

			UINT32 curIndexOffset = 0;
			for (auto curRenderData : *renderData)
			{
				mAlphaPickingMaterial.mTexture.set(curRenderData.texture);
				rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, mAlphaPickingMaterial.mFragParams);

				rs.drawIndexed(curIndexOffset, curRenderData.count * 6, mesh->getVertexOffset(), curRenderData.count * 4);
				curIndexOffset += curRenderData.count * 6;

			}
		}

		mesh->_notifyUsedOnGPU();
	}
}