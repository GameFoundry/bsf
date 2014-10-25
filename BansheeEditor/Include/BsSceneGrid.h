#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector3.h"
#include "BsColor.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	class SceneGrid
	{
	public:
		SceneGrid();

		void setOrigin(const Vector3& origin);
		void setSize(UINT32 size);
		void setSpacing(float spacing);
		void setMajorAxisSpacing(UINT32 spacing);
		void setAxisMarkerSpacing(UINT32 spacing);

		void render(const CameraPtr& camera, DrawList& drawList);
	private:
		void updateGridMesh();

		HMesh mGridMesh;
		HMaterial mGridMaterial;
		GpuParamMat4 mViewProjParam;
		VertexDataDescPtr mVertexDesc;

		Vector3 mOrigin;
		float mSpacing = 1.0f;
		UINT32 mSize = 256;
		UINT32 mMajorAxisSpacing = 10;
		UINT32 mAxisMarkerSpacing = 25;

		static const float LINE_WIDTH;
		static const float LINE_BORDER_WIDTH;
		static const float MAJOR_AXIS_WIDTH;
		static const float MAJOR_AXIS_BORDER_WIDTH;
		static const float AXIS_MARKER_WIDTH;
		static const float AXIS_MARKER_BORDER_WIDTH;
		static const Color AXIS_X_MARKER_COLOR;
		static const Color AXIS_Z_MARKER_COLOR;
	};
}