#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector3.h"
#include "BsColor.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles rendering of the grid in the scene view.
	 */
	class SceneGrid
	{
	public:
		SceneGrid();

		/**
		 * @brief	Sets the grid origin in world coordinates.
		 */
		void setOrigin(const Vector3& origin);

		/**
		 * @brief	Sets the total width/height of the grid in XZ plane.
		 */
		void setSize(UINT32 size);
		
		/**
		 * @brief	Sets the spacing between grid lines.
		 */
		void setSpacing(float spacing);

		/**
		 * @brief	Changes the active editor settings. Grid properties
		 *			will be updated internally when editor settings change.
		 */
		void setSettings(const EditorSettingsPtr& settings);

		/**
		 * @brief	Triggered by the renderer when rendering the specified camera.
		 *
		 * @param	camera		Camera about to be rendered.
		 * @param	drawList	Draw list we can use to queue our render commands in.
		 *
		 * @note	Internal method.
		 */
		void _render(const CameraHandlerPtr& camera, DrawList& drawList);

		/**
		 * @brief	Called once per frame.
		 *
		 * @note	Internal method.
		 */
		void update();
	private:
		/**
		 * @brief	Rebuilds the scene grid mesh. Call this whenever grid parameters change.
		 */
		void updateGridMesh();

		/**
		 * @brief	Updates internal grid parameters from the attached settings object.
		 */
		void updateFromEditorSettings();

		HMesh mGridMesh;
		HMaterial mGridMaterial;
		MaterialParamMat4 mViewProjParam;
		MaterialParamVec4 mWorldCameraPosParam;
		MaterialParamColor mGridColorParam;
		MaterialParamFloat mGridSpacingParam;
		MaterialParamFloat mGridBorderWidthParam;
		MaterialParamFloat mGridFadeOutStartParam;
		MaterialParamFloat mGridFadeOutEndParam;
		VertexDataDescPtr mVertexDesc;

		EditorSettingsPtr mSettings;
		UINT32 mSettingsHash = 0;

		Vector3 mOrigin;
		float mSpacing = 1.0f;
		UINT32 mSize = 256;
		UINT32 mMajorAxisSpacing = 10;
		UINT32 mAxisMarkerSpacing = 25;

		static const Color GRID_LINE_COLOR;
		static const float LINE_WIDTH;
		static const float LINE_BORDER_WIDTH;
		static const float MAJOR_AXIS_WIDTH;
		static const float MAJOR_AXIS_BORDER_WIDTH;
		static const float AXIS_MARKER_WIDTH;
		static const float AXIS_MARKER_BORDER_WIDTH;
		static const Color AXIS_X_MARKER_COLOR;
		static const Color AXIS_Z_MARKER_COLOR;
		static const float FADE_OUT_START;
		static const float FADE_OUT_END;
	};
}