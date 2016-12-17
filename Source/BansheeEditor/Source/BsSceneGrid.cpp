//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSceneGrid.h"
#include "BsMath.h"
#include "BsShapeMeshes3D.h"
#include "BsVertexDataDesc.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsBuiltinEditorResources.h"
#include "BsCCamera.h"
#include "BsRect3.h"
#include "BsCoreThread.h"
#include "BsEditorSettings.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsRendererUtility.h"

namespace bs
{
	const Color SceneGridRenderer::GRID_LINE_COLOR = Color(0.5f, 0.5f, 0.5f);
	const float SceneGridRenderer::LINE_WIDTH = 0.025f;
	const float SceneGridRenderer::LINE_BORDER_WIDTH = 0.00075f;
	const float SceneGridRenderer::FADE_OUT_START = 5.0f;
	const float SceneGridRenderer::FADE_OUT_END = 40.0f;

	SceneGrid::SceneGrid(const SPtr<Camera>& camera)
		:mCoreDirty(true)
	{
		mVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mVertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

		HMaterial gridMaterial = BuiltinEditorResources::instance().createSceneGridMaterial();

		SceneGridRenderer::InitData initData;
		initData.material = gridMaterial->getCore();
		initData.camera = camera->getCore();

		mRenderer = RendererExtension::create<SceneGridRenderer>(initData);
		updateGridMesh();
	}

	SceneGrid::~SceneGrid()
	{ }

	void SceneGrid::setSize(UINT32 size)
	{
		if (mSize != size)
		{
			mSize = size;
			updateGridMesh();
		}
	}

	void SceneGrid::setSpacing(float spacing)
	{
		if (mSpacing != spacing)
		{
			mSpacing = spacing;
			mCoreDirty = true;
		}
	}

	void SceneGrid::setMode(GridMode mode)
	{
		if(mMode != mode)
		{
			mMode = mode;
			updateGridMesh();
		}
	}

	void SceneGrid::setSettings(const SPtr<EditorSettings>& settings)
	{
		mSettings = settings;
		updateFromEditorSettings();
	}

	void SceneGrid::_update()
	{
		if (mSettings != nullptr && mSettingsHash != mSettings->getHash())
			updateFromEditorSettings();

		if (mCoreDirty)
		{
			Vector3 gridPlaneNormal = Vector3(0.0f, 1.0f, 0.0f);

			switch (mMode)
			{
			case GridMode::OrthoX:
			case GridMode::OrthoNegX:
				gridPlaneNormal = Vector3(1.0f, 0.0f, 0.0f);
				break;
			case GridMode::OrthoZ:
			case GridMode::OrthoNegZ:
				gridPlaneNormal = Vector3(0.0f, 0.0f, 1.0f);
				break;
			default:
				break;
			}

			SceneGridRenderer* renderer = mRenderer.get();
			gCoreThread().queueCommand(
				std::bind(&SceneGridRenderer::updateData, renderer, mGridMesh->getCore(), mSpacing, 
				mMode == GridMode::Perspective, gridPlaneNormal));

			mCoreDirty = false;
		}
	}

	void SceneGrid::updateFromEditorSettings()
	{
		setSize(mSettings->getGridSize());
		setSpacing(mSettings->getGridSpacing());

		mSettingsHash = mSettings->getHash();
	}

	void SceneGrid::updateGridMesh()
	{
		std::array<Vector3, 2> axes;
		Vector3 origin;

		switch(mMode)
		{
		case GridMode::Perspective:
			axes[0] = Vector3::UNIT_X;
			axes[1] = Vector3::UNIT_Z;
			origin = Vector3::ZERO;
			break;
		case GridMode::OrthoX:
			axes[0] = Vector3::UNIT_Y;
			axes[1] = Vector3::UNIT_Z;
			origin = Vector3(500.0f, 0.0f, 0.0f);
			break;
		case GridMode::OrthoY:
			axes[0] = Vector3::UNIT_X;
			axes[1] = Vector3::UNIT_Z;
			origin = Vector3(0.0f, 500.0f, 0.0f);
			break;
		case GridMode::OrthoZ:
			axes[0] = Vector3::UNIT_X;
			axes[1] = Vector3::UNIT_Y;
			origin = Vector3(0.0f, 0.0f, 500.0f);
			break;
		case GridMode::OrthoNegX:
			axes[0] = Vector3::UNIT_Y;
			axes[1] = Vector3::UNIT_Z;
			origin = Vector3(-500.0f, 0.0f, 0.0f);
			break;
		case GridMode::OrthoNegY:
			axes[0] = Vector3::UNIT_X;
			axes[1] = Vector3::UNIT_Z;
			origin = Vector3(0.0f, -500.0f, 0.0f);
			break;
		case GridMode::OrthoNegZ:
			axes[0] = Vector3::UNIT_X;
			axes[1] = Vector3::UNIT_Y;
			origin = Vector3(0.0f, 0.0f, -500.0f);
			break;
		}

		std::array<float, 2> extents;
		extents[0] = mSize * 0.5f;
		extents[1] = mSize * 0.5f;

		Rect3 quad(origin, axes, extents);
		SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(8, 12, mVertexDesc);

		ShapeMeshes3D::solidQuad(quad, meshData, 0, 0);
		mGridMesh = Mesh::create(meshData);
		mCoreDirty = true;
	}

	SceneGridRenderer::SceneGridRenderer()
		:RendererExtension(RenderLocation::PostLightPass, -5)
	{ }

	void SceneGridRenderer::initialize(const Any& data)
	{
		const InitData& initData = any_cast_ref<InitData>(data);

		mCamera = initData.camera;
		mGridMaterial = initData.material;
		mMaterialParams = initData.material->createParamsSet();

		mViewProjParam = mGridMaterial->getParamMat4("matViewProj");
		mWorldCameraPosParam = mGridMaterial->getParamVec4("worldCameraPos");
		mGridColorParam = mGridMaterial->getParamColor("gridColor");
		mGridSpacingParam = mGridMaterial->getParamFloat("gridSpacing");
		mGridBorderWidthParam = mGridMaterial->getParamFloat("gridBorderWidth");
		mGridFadeOutStartParam = mGridMaterial->getParamFloat("gridFadeOutStart");
		mGridFadeOutEndParam = mGridMaterial->getParamFloat("gridFadeOutEnd");
		mGridMaterial->getParam("gridPlaneNormal", mGridPlaneNormalParam);		
	}

	void SceneGridRenderer::updateData(const SPtr<MeshCore>& mesh, float spacing, bool fadeGrid, const Vector3& gridPlaneNormal)
	{
		mGridMesh = mesh;
		mSpacing = spacing;
		mFadeGrid = fadeGrid;
		mGridPlaneNormal = gridPlaneNormal;
	}

	bool SceneGridRenderer::check(const CameraCore& camera)
	{
		return mCamera.get() == &camera;
	}

	void SceneGridRenderer::render(const CameraCore& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 projMatrix = mCamera->getProjectionMatrixRS();
		Matrix4 viewMatrix = mCamera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrix * viewMatrix;
		mViewProjParam.set(viewProjMatrix);

		mWorldCameraPosParam.set(Vector4(mCamera->getPosition(), 1.0f));
		mGridColorParam.set(GRID_LINE_COLOR);
		mGridSpacingParam.set(mSpacing);
		mGridBorderWidthParam.set(LINE_BORDER_WIDTH);
		mGridPlaneNormalParam.set(mGridPlaneNormal);

		if (mFadeGrid)
		{
			mGridFadeOutStartParam.set(FADE_OUT_START);
			mGridFadeOutEndParam.set(FADE_OUT_END);
		}
		else
		{
			mGridFadeOutStartParam.set(1000.0f);
			mGridFadeOutEndParam.set(1500.0f);
		}

		mGridMaterial->updateParamsSet(mMaterialParams);

		gRendererUtility().setPass(mGridMaterial, 0);
		gRendererUtility().setPassParams(mMaterialParams);
		gRendererUtility().draw(mGridMesh, mGridMesh->getProperties().getSubMesh(0));
	}
}