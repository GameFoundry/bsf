#include "BsEditorSettings.h"

namespace BansheeEngine
{
	EditorSettings::EditorSettings()
		:mMoveSnapActive(false), mRotateSnapActive(false), mScaleSnapActive(false), mMoveSnap(0.1f),
		mRotationSnap(20.0f), mScaleSnap(0.1f), mGridSize(256), mGridAxisSpacing(1.0f), mGridMajorAxisSpacing(10),
		mGridAxisMarkerSpacing(25), mHandleSize(0.15f), mHash(0)
	{ }
}