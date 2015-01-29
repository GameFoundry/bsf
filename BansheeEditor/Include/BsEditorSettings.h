#pragma once

#include "BsEditorPrerequisites.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorSettings
	{
	public:
		EditorSettings();

		bool getMoveHandleSnapActive() const { return mMoveSnapActive; }
		bool getRotateHandleSnapActive() const { return mRotateSnapActive; }

		float getMoveHandleSnap() const { return mMoveSnap; }
		Degree getRotationHandleSnap() const { return mRotationSnap; }

		UINT32 getGridSize() const { return mGridSize; }
		float getGridSpacing() const { return mGridAxisSpacing; }
		UINT32 getGridMajorAxisSpacing() const { return mGridMajorAxisSpacing; }
		UINT32 getGridAxisMarkerSpacing() const { return mGridAxisMarkerSpacing; }

		float getHandleSize() const { return mHandleSize; }

		UINT32 getActiveSceneTool() const { return mActiveSceneTool; }
		UINT32 getActiveCoordinateMode() const { return mActiveCoordinateMode; }
		UINT32 getActivePivotMode() const { return mActivePivotMode; }

		void setMoveHandleSnapActive(bool snapActive) { mMoveSnapActive = snapActive; markAsDirty(); }
		void setRotateHandleSnapActive(bool snapActive) { mRotateSnapActive = snapActive; markAsDirty(); }

		void setMoveHandleSnap(float value) { mMoveSnap = value; markAsDirty(); }
		void setRotationHandleSnap(Degree value) { mRotationSnap = value; markAsDirty(); }

		void setGridSize(UINT32 value) { mGridSize = value; markAsDirty(); }
		void setGridSpacing(float value) { mGridAxisSpacing = value; markAsDirty(); }
		void setGridMajorAxisSpacing(UINT32 value) { mGridMajorAxisSpacing = value; markAsDirty(); }
		void setGridAxisMarkerSpacing(UINT32 value) { mGridMajorAxisSpacing = value; markAsDirty(); }

		void setHandleSize(float value) { mHandleSize = value; markAsDirty(); }

		void setActiveSceneTool(UINT32 value) { mActiveSceneTool = value; markAsDirty(); }
		void setActiveCoordinateMode(UINT32 value) { mActiveCoordinateMode = value; markAsDirty(); }
		void setActivePivotMode(UINT32 value) { mActivePivotMode = value; markAsDirty(); }

		UINT32 getHash() const { return mHash; }

	private:
		void markAsDirty() const { mHash++; }

		bool mMoveSnapActive;
		bool mRotateSnapActive;

		float mMoveSnap;
		Degree mRotationSnap;

		UINT32 mGridSize;
		float mGridAxisSpacing;
		UINT32 mGridMajorAxisSpacing;
		UINT32 mGridAxisMarkerSpacing;

		UINT32 mActiveSceneTool;
		UINT32 mActiveCoordinateMode;
		UINT32 mActivePivotMode;

		float mHandleSize;

		mutable UINT32 mHash;
	};
}