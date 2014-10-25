#pragma once

#include "BsEditorPrerequisites.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT ProjectSettings
	{
	public:
		float getMoveHandleSnap() const { return mMoveSnap; }
		Degree getRotationHandleSnap() const { return mRotationSnap; }
		float getScaleHandleSnap() const { return mScaleSnap; }

		UINT32 getGridSize() const { return mGridSize; }
		float getGridSpacing() const { return mGridAxisSpacing; }
		UINT32 getGridMajorAxisSpacing() const { return mGridMajorAxisSpacing; }
		UINT32 getGridAxisMarkerSpacing() const { return mGridAxisMarkerSpacing; }

		float getHandleSize() const { return mHandleSize; }

		void setMoveHandleSnap(float value) { mMoveSnap = value; }
		void setRotationHandleSnap(Degree value) { mRotationSnap = value; }
		void setScaleHandleSnap(float value) { mScaleSnap = value; }

		void setGridSize(UINT32 value) { mGridSize = value; }
		void setGridSpacing(float value) { mGridAxisSpacing = value; }
		void setGridMajorAxisSpacing(UINT32 value) { mGridMajorAxisSpacing = value; }
		void setGridAxisMarkerSpacing(UINT32 value) { mGridMajorAxisSpacing = value; }

		void setHandleSize(float value) { mHandleSize = value; }

	private:
		float mMoveSnap = 0.1f;
		Degree mRotationSnap = Degree(20.0f);
		float mScaleSnap = 0.1f;

		UINT32 mGridSize = 256;
		float mGridAxisSpacing = 1.0f;
		UINT32 mGridMajorAxisSpacing = 10;
		UINT32 mGridAxisMarkerSpacing = 25;

		float mHandleSize = 20.0f;
	};
}