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
		bool getScaleHandleSnapActive() const { return mScaleSnapActive; }

		float getMoveHandleSnap() const { return mMoveSnap; }
		Degree getRotationHandleSnap() const { return mRotationSnap; }
		float getScaleHandleSnap() const { return mScaleSnap; }

		UINT32 getGridSize() const { return mGridSize; }
		float getGridSpacing() const { return mGridAxisSpacing; }
		UINT32 getGridMajorAxisSpacing() const { return mGridMajorAxisSpacing; }
		UINT32 getGridAxisMarkerSpacing() const { return mGridAxisMarkerSpacing; }

		float getHandleSize() const { return mHandleSize; }

		void setMoveHandleSnapActive(bool snapActive) { mMoveSnapActive = snapActive; markAsDirty(); }
		void setRotateHandleSnapActive(bool snapActive) { mRotateSnapActive = snapActive; markAsDirty(); }
		void setScaleHandleSnapActive(bool snapActive) { mScaleSnapActive = snapActive; markAsDirty(); }

		void setMoveHandleSnap(float value) { mMoveSnap = value; markAsDirty(); }
		void setRotationHandleSnap(Degree value) { mRotationSnap = value; markAsDirty(); }
		void setScaleHandleSnap(float value) { mScaleSnap = value; markAsDirty(); }

		void setGridSize(UINT32 value) { mGridSize = value; markAsDirty(); }
		void setGridSpacing(float value) { mGridAxisSpacing = value; markAsDirty(); }
		void setGridMajorAxisSpacing(UINT32 value) { mGridMajorAxisSpacing = value; markAsDirty(); }
		void setGridAxisMarkerSpacing(UINT32 value) { mGridMajorAxisSpacing = value; markAsDirty(); }

		void setHandleSize(float value) { mHandleSize = value; markAsDirty(); }

		UINT32 getHash() const { return mHash; }

	private:
		void markAsDirty() const { mHash++; }

		bool mMoveSnapActive;
		bool mRotateSnapActive;
		bool mScaleSnapActive;

		float mMoveSnap;
		Degree mRotationSnap;
		float mScaleSnap;

		UINT32 mGridSize;
		float mGridAxisSpacing;
		UINT32 mGridMajorAxisSpacing;
		UINT32 mGridAxisMarkerSpacing;

		float mHandleSize;

		mutable UINT32 mHash;
	};
}