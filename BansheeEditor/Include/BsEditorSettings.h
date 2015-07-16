#pragma once

#include "BsEditorPrerequisites.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains various globally accessible editor preferences.
	 */
	class BS_ED_EXPORT EditorSettings
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
		 * @brief	Returns a hash value that may be used for checking if any internal settings were
		 *			modified.
		 */
		UINT32 getHash() const { return mHash; }

	private:
		/**
		 * @brief	Marks the object as dirty so that outside objects know when to update.
		 */
		void markAsDirty() const { mHash++; }

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

		mutable UINT32 mHash;
	};
}