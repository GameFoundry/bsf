#pragma once

#include "BsEditorPrerequisites.h"
#include "BsSettings.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	struct RecentProject;

	/**
	 * @brief	Contains various globally accessible editor preferences.
	 */
	class BS_ED_EXPORT EditorSettings : public Settings
	{
	public:
		EditorSettings();

		/**
		 * @brief	Checks is snapping enabled for move handles in scene view.
		 */
		bool getMoveHandleSnapActive() const { return mMoveSnapActive; }

		/**
		 * @brief	Checks is angle snapping enabled for rotate handles in scene view.
		 */
		bool getRotateHandleSnapActive() const { return mRotateSnapActive; }

		/**
		 * @brief	Gets the snap amount if move snapping is enabled. All move handles
		 *			will move in multiples of this amount.
		 */
		float getMoveHandleSnap() const { return mMoveSnap; }

		/**
		 * @brief	Gets the snap amount if rotate snapping is enabled. All rotate handles
		 *			will rotate in multiples of this amount.
		 */
		Degree getRotationHandleSnap() const { return mRotationSnap; }

		/**
		 * @brief	Returns the size that determines to total size of the scene view grid (its width and height).
		 */
		UINT32 getGridSize() const { return mGridSize; }

		/**
		 * @brief	Returns the distance between scene view grid lines.
		 */
		float getGridSpacing() const { return mGridAxisSpacing; }

		/**
		 * @brief	Gets the default size of all scene view handles.
		 */
		float getHandleSize() const { return mHandleSize; }

		/**
		 * @brief	Returns the currently active scene view tool (e.g. move, rotate, etc.)
		 */
		UINT32 getActiveSceneTool() const { return mActiveSceneTool; }

		/**
		 * @brief	Returns the currently active coordinate mode for scene view (e.g. global/local)
		 */
		UINT32 getActiveCoordinateMode() const { return mActiveCoordinateMode; }

		/**
		 * @brief	Returns the currently active pivot mode for scene view (e.g. pivot/center)
		 */
		UINT32 getActivePivotMode() const { return mActivePivotMode; }

		/**
		 * @brief	Retrieves the path to the last project open in the editor.
		 */
		Path getLastOpenProject() const { return mLastOpenProject; }

		/**
		 * @brief	Retrieves whether the last open project should be automatically loaded
		 *			on editor start up.
		 */
		bool getAutoLoadLastProject() const { return mAutoLoadLastProject; }

		/**
		 * @brief	Retrieves a list of most recently loaded project paths and their last access times.
		 */
		const Vector<RecentProject>& getRecentProjects() const { return mRecentProjects; }

		/**
		 * @brief	Enables/disables snapping for move handles in scene view.
		 */
		void setMoveHandleSnapActive(bool snapActive) { mMoveSnapActive = snapActive; markAsDirty(); }

		/**
		 * @brief	Enables/disables angle snapping for rotate handles in scene view.
		 */
		void setRotateHandleSnapActive(bool snapActive) { mRotateSnapActive = snapActive; markAsDirty(); }

		/**
		 * @brief	Sets the move snap amount. All move handles will move in multiples of this amount.
		 */
		void setMoveHandleSnap(float value) { mMoveSnap = value; markAsDirty(); }

		/**
		 * @brief	Sets the rotate snap amount. All rotate handles will rotate in multiples of this amount.
		 */
		void setRotationHandleSnap(Degree value) { mRotationSnap = value; markAsDirty(); }

		/**
		 * @brief	Sets the size that determines to total size of the scene view grid (its width and height).
		 */
		void setGridSize(UINT32 value) { mGridSize = value; markAsDirty(); }

		/**
		 * @brief	Sets the distance between scene view grid lines.
		 */
		void setGridSpacing(float value) { mGridAxisSpacing = value; markAsDirty(); }

		/**
		 * @brief	Sets the default size of all scene view handles.
		 */
		void setHandleSize(float value) { mHandleSize = value; markAsDirty(); }

		/**
		 * @brief	Changes the currently active scene view tool (e.g. move, rotate, etc.)
		 */
		void setActiveSceneTool(UINT32 value) { mActiveSceneTool = value; markAsDirty(); }

		/**
		 * @brief	Changes the currently active coordinate mode for scene view (e.g. global/local)
		 */
		void setActiveCoordinateMode(UINT32 value) { mActiveCoordinateMode = value; markAsDirty(); }

		/**
		 * @brief	Changes the currently active pivot mode for scene view (e.g. pivot/center)
		 */
		void setActivePivotMode(UINT32 value) { mActivePivotMode = value; markAsDirty(); }

		/**
		 * @brief	Sets the path to the last project open in the editor.
		 */
		void setLastOpenProject(const Path& value) { mLastOpenProject = value; markAsDirty(); }

		/**
		 * @brief	Sets whether the last open project should be automatically loaded
		 *			on editor start up.
		 */
		void setAutoLoadLastProject(bool value) { mAutoLoadLastProject = value; markAsDirty(); }

		/**
		 * @brief	Sets a list of most recently loaded project paths and their last access times.
		 */
		void setRecentProjects(const Vector<RecentProject>& value) { mRecentProjects = value; markAsDirty(); }

	private:
		bool mMoveSnapActive;
		bool mRotateSnapActive;

		float mMoveSnap;
		Degree mRotationSnap;

		UINT32 mGridSize;
		float mGridAxisSpacing;

		UINT32 mActiveSceneTool;
		UINT32 mActiveCoordinateMode;
		UINT32 mActivePivotMode;

		float mHandleSize;

		Path mLastOpenProject;
		bool mAutoLoadLastProject;
		Vector<RecentProject> mRecentProjects;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class EditorSettingsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Data about a recently loaded project.
	 */
	struct RecentProject
	{
		Path path;
		UINT64 accessTimestamp;
	};
}