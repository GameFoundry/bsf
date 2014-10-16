#include "BsSceneGrid.h"
#include "BsMath.h"
#include "BsShapeMeshes3D.h"
#include "BsVertexDataDesc.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsDrawList.h"
#include "BsBuiltinEditorResources.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	const float SceneGrid::LINE_WIDTH = 0.025f;
	const float SceneGrid::LINE_BORDER_WIDTH = 0.005f;
	const float SceneGrid::MAJOR_AXIS_WIDTH = 0.075f;
	const float SceneGrid::MAJOR_AXIS_BORDER_WIDTH = 0.015f;
	const float SceneGrid::AXIS_MARKER_WIDTH = 0.1f;
	const float SceneGrid::AXIS_MARKER_BORDER_WIDTH = 0.02f;
	const Color SceneGrid::AXIS_X_MARKER_COLOR = Color::Red;
	const Color SceneGrid::AXIS_Z_MARKER_COLOR = Color::Blue;

	SceneGrid::SceneGrid()
	{
		mVertexDesc = bs_shared_ptr<VertexDataDesc>();
		mVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mGridMaterial = BuiltinEditorResources::instance().createSceneGridMaterial();
		mViewProjParam = mGridMaterial->getParamMat4("matViewProj");

		updateGridMesh();
	}

	void SceneGrid::setOrigin(const Vector3& origin)
	{
		mOrigin = origin;
		updateGridMesh();
	}

	void SceneGrid::setSize(float size)
	{
		mSize = size;
		updateGridMesh();
	}

	void SceneGrid::setSpacing(float spacing)
	{
		mSpacing = spacing;
		updateGridMesh();
	}

	void SceneGrid::setMajorAxisSpacing(UINT32 spacing)
	{
		mMajorAxisSpacing = spacing;
		updateGridMesh();
	}

	void SceneGrid::setAxisMarkerSpacing(UINT32 spacing)
	{
		mAxisMarkerSpacing = spacing;
		updateGridMesh();
	}

	void SceneGrid::render(const CameraPtr& camera, DrawList& drawList)
	{
		MaterialPtr mat = mGridMaterial.getInternalPtr();
		MeshPtr mesh = mGridMesh.getInternalPtr();

		Matrix4 projMatrix = camera->getProjectionMatrixRS();
		Matrix4 viewMatrix = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrix * viewMatrix;
		mViewProjParam.set(viewProjMatrix);

		drawList.add(mat, mesh, 0, Vector3::ZERO);
	}

	void SceneGrid::updateGridMesh()
	{
		UINT32 numLines = (UINT32)Math::roundToInt(mSize / mSpacing) - 1;
		if (numLines % 2 != 0)
			numLines++;

		INT32 originX = Math::roundToInt(mOrigin.x / mSpacing);
		INT32 originZ = Math::roundToInt(mOrigin.y / mSpacing);

		INT32 startX = originX - numLines / 2;
		INT32 startZ = originZ - numLines / 2;

		float minX = startX * mSpacing;
		float minZ = startZ * mSpacing;

		float maxX = (startX + numLines) * mSpacing;
		float maxZ = (startZ + numLines) * mSpacing;

		UINT32 totalNumVertices = ShapeMeshes3D::NUM_VERTICES_AA_LINE * (numLines + 1) * 2;
		UINT32 totalNumIndices = ShapeMeshes3D::NUM_INDICES_AA_LINE * (numLines + 1) * 2;

		MeshDataPtr meshData = bs_shared_ptr<MeshData, PoolAlloc>(totalNumVertices, totalNumIndices, mVertexDesc);
		UINT32 vertexOffset = 0;
		UINT32 indexOffset = 0;

		for (UINT32 i = 0; i <= numLines; i++)
		{
			INT32 x = startX + i;
			float linePosX = x * mSpacing;

			Vector3 lineStartX(linePosX, 0, minZ);
			Vector3 lineEndX(linePosX, 0, maxZ);

			if (x % mAxisMarkerSpacing == 0)
			{
				ShapeMeshes3D::antialiasedLine(lineStartX, lineEndX, Vector3::UNIT_Y, AXIS_MARKER_WIDTH, 
					AXIS_MARKER_BORDER_WIDTH, AXIS_X_MARKER_COLOR, meshData, vertexOffset, indexOffset);
			}
			else if (x % mMajorAxisSpacing == 0)
			{
				ShapeMeshes3D::antialiasedLine(lineStartX, lineEndX, Vector3::UNIT_Y, MAJOR_AXIS_WIDTH,
					MAJOR_AXIS_BORDER_WIDTH, Color::White, meshData, vertexOffset, indexOffset);
			}
			else
			{
				ShapeMeshes3D::antialiasedLine(lineStartX, lineEndX, Vector3::UNIT_Y, LINE_WIDTH,
					LINE_BORDER_WIDTH, Color::White, meshData, vertexOffset, indexOffset);
			}

			vertexOffset += ShapeMeshes3D::NUM_VERTICES_AA_LINE;
			indexOffset += ShapeMeshes3D::NUM_INDICES_AA_LINE;

			INT32 z = startZ + i;
			float linePosZ = z * mSpacing;

			Vector3 lineStartZ(minX, 0, linePosZ);
			Vector3 lineEndZ(maxX, 0, linePosZ);

			if (z % mAxisMarkerSpacing == 0)
			{
				ShapeMeshes3D::antialiasedLine(lineStartZ, lineEndZ, Vector3::UNIT_Y, AXIS_MARKER_WIDTH,
					AXIS_MARKER_BORDER_WIDTH, AXIS_Z_MARKER_COLOR, meshData, vertexOffset, indexOffset);
			}
			else if (z % mMajorAxisSpacing == 0)
			{
				ShapeMeshes3D::antialiasedLine(lineStartZ, lineEndZ, Vector3::UNIT_Y, MAJOR_AXIS_WIDTH,
					MAJOR_AXIS_BORDER_WIDTH, Color::White, meshData, vertexOffset, indexOffset);
			}
			else
			{
				ShapeMeshes3D::antialiasedLine(lineStartZ, lineEndZ, Vector3::UNIT_Y, LINE_WIDTH,
					LINE_BORDER_WIDTH, Color::White, meshData, vertexOffset, indexOffset);
			}

			vertexOffset += ShapeMeshes3D::NUM_VERTICES_AA_LINE;
			indexOffset += ShapeMeshes3D::NUM_INDICES_AA_LINE;
		}

		mGridMesh = Mesh::create(meshData);
	}
}