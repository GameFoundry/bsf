#include "BsSceneGrid.h"
#include "BsMath.h"
#include "BsShapeMeshes3D.h"
#include "BsVertexDataDesc.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsDrawList.h"
#include "BsBuiltinEditorResources.h"
#include "BsCamera.h"
#include "BsRect3.h"
#include "BsEditorSettings.h"

namespace BansheeEngine
{
	const Color SceneGrid::GRID_LINE_COLOR = Color(0.5f, 0.5f, 0.5f);
	const float SceneGrid::LINE_WIDTH = 0.025f;
	const float SceneGrid::LINE_BORDER_WIDTH = 0.00075f;
	const float SceneGrid::MAJOR_AXIS_WIDTH = 0.075f;
	const float SceneGrid::MAJOR_AXIS_BORDER_WIDTH = 0.015f;
	const float SceneGrid::AXIS_MARKER_WIDTH = 0.1f;
	const float SceneGrid::AXIS_MARKER_BORDER_WIDTH = 0.02f;
	const Color SceneGrid::AXIS_X_MARKER_COLOR = Color::Red;
	const Color SceneGrid::AXIS_Z_MARKER_COLOR = Color::Blue;
	const float SceneGrid::FADE_OUT_START = 5.0f;
	const float SceneGrid::FADE_OUT_END = 40.0f;

	SceneGrid::SceneGrid()
	{
		mVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mVertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

		mGridMaterial = BuiltinEditorResources::instance().createSceneGridMaterial();
		mViewProjParam = mGridMaterial->getParamMat4("matViewProj");
		mWorldCameraPosParam = mGridMaterial->getParamVec4("worldCameraPos");
		mGridColorParam = mGridMaterial->getParamColor("gridColor");
		mGridSpacingParam = mGridMaterial->getParamFloat("gridSpacing");
		mGridBorderWidthParam = mGridMaterial->getParamFloat("gridBorderWidth");
		mGridFadeOutStartParam = mGridMaterial->getParamFloat("gridFadeOutStart");
		mGridFadeOutEndParam = mGridMaterial->getParamFloat("gridFadeOutEnd");

		updateGridMesh();
	}

	void SceneGrid::setOrigin(const Vector3& origin)
	{
		if (mOrigin != origin)
		{
			mOrigin = origin;
			updateGridMesh();
		}
	}

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
			updateGridMesh();
		}
	}

	void SceneGrid::setSettings(const EditorSettingsPtr& settings)
	{
		mSettings = settings;
		updateFromEditorSettings();
	}

	void SceneGrid::update()
	{
		if (mSettings != nullptr && mSettingsHash != mSettings->getHash())
			updateFromEditorSettings();
	}

	void SceneGrid::_render(const CameraHandlerPtr& camera, DrawList& drawList)
	{
		MaterialPtr mat = mGridMaterial.getInternalPtr();
		MeshPtr mesh = mGridMesh.getInternalPtr();

		Matrix4 projMatrix = camera->getProjectionMatrixRS();
		Matrix4 viewMatrix = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrix * viewMatrix;
		mViewProjParam.set(viewProjMatrix);

		mWorldCameraPosParam.set(Vector4(camera->getPosition(), 1.0f));
		mGridColorParam.set(GRID_LINE_COLOR);
		mGridSpacingParam.set(mSpacing);
		mGridBorderWidthParam.set(LINE_BORDER_WIDTH);
		mGridFadeOutStartParam.set(FADE_OUT_START);
		mGridFadeOutEndParam.set(FADE_OUT_END);

		drawList.add(mat, mesh, 0, Vector3::ZERO);
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
		axes[0] = Vector3::UNIT_X;
		axes[1] = Vector3::UNIT_Z;

		std::array<float, 2> extents;
		extents[0] = mSize * 0.5f;
		extents[1] = mSize * 0.5f;

		Rect3 quad(mOrigin, axes, extents);
		MeshDataPtr meshData = bs_shared_ptr_new<MeshData>(8, 12, mVertexDesc);

		ShapeMeshes3D::solidQuad(quad, meshData, 0, 0);
		mGridMesh = Mesh::create(meshData);
	}
}