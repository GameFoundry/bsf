#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector3.h"
#include "BsColor.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	class SceneGridCore;

	/**
	 * @brief	Handles rendering of the grid in the scene view.
	 */
	class SceneGrid
	{
	public:
		SceneGrid(const CameraHandlerPtr& camera);
		~SceneGrid();

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
		 * @brief	Called once per frame.
		 *
		 * @note	Internal method.
		 */
		void update();
	private:
		/**
		 * @brief	Updates internal grid parameters from the attached settings object.
		 */
		void updateFromEditorSettings();

		/**
		 * @brief	Rebuilds the scene grid mesh. Call this whenever grid parameters change.
		 */
		void updateGridMesh();

		/**
		 * @brief	Initializes the core thread portion of the scene grid renderer.
		 *
		 * @param	material	Material used for drawing the grid.
		 * @param	camera		Camera to render the scene grid to.
		 */
		void initializeCore(const SPtr<CameraHandlerCore>& camera, const SPtr<MaterialCore>& material);

		/**
		 * @brief	Destroys the core thread portion of the draw manager.
		 */
		void destroyCore(SceneGridCore* core);

		Vector3 mOrigin;
		float mSpacing = 1.0f;
		UINT32 mSize = 256;
		bool mCoreDirty;

		EditorSettingsPtr mSettings;
		UINT32 mSettingsHash = 0;

		HMesh mGridMesh;
		VertexDataDescPtr mVertexDesc;
		std::atomic<SceneGridCore*> mCore;
	};

	/**
	 * @brief	Core thread portion of the scene grid. Handles interaction with the renderer.
	 */
	class SceneGridCore
	{
	public:
		SceneGridCore() { }
		~SceneGridCore();

	private:
		friend class SceneGrid;

		/**
		 * @brief	Initializes the object. Must be called right after construction and before any use.
		 *
		 * @param	material	Material used for drawing the grid.
		 * @param	camera		Camera to render the scene grid to.
		 */
		void initialize(const SPtr<CameraHandlerCore>& camera, const SPtr<MaterialCore>& material);

		/**
		 * @brief	Updates the grid mesh to render.
		 * 			
		 * @param	mesh		Grid mesh to render.
		 * @param	scpacing	Spacing between the grid lines.
		 */
		void updateData(const SPtr<MeshCore>& mesh, float spacing);

		/**
		 * @brief	Callback triggered by the renderer, actually draws the grid mesh.
		 */
		void render();

		SPtr<CameraHandlerCore> mCamera;
		SPtr<MeshCore> mGridMesh;
		SPtr<MaterialCore> mGridMaterial;
		float mSpacing = 1.0f;

		MaterialParamMat4Core mViewProjParam;
		MaterialParamVec4Core mWorldCameraPosParam;
		MaterialParamColorCore mGridColorParam;
		MaterialParamFloatCore mGridSpacingParam;
		MaterialParamFloatCore mGridBorderWidthParam;
		MaterialParamFloatCore mGridFadeOutStartParam;
		MaterialParamFloatCore mGridFadeOutEndParam;

		static const Color GRID_LINE_COLOR;
		static const float LINE_WIDTH;
		static const float LINE_BORDER_WIDTH;
		static const float FADE_OUT_START;
		static const float FADE_OUT_END;
	};
}