#include "BsEditorSettings.h"
#include "BsEditorSettingsRTTI.h"

namespace BansheeEngine
{
	EditorSettings::EditorSettings()
		:mMoveSnapActive(false), mRotateSnapActive(false), mMoveSnap(0.1f), mRotationSnap(20.0f),
		mGridSize(256), mGridAxisSpacing(1.0f), mHandleSize(0.10f), mActiveSceneTool(1 /* Move */),
		mActiveCoordinateMode(0), mActivePivotMode(0)
	{ }
	
	RTTITypeBase* EditorSettings::getRTTIStatic()
	{
		return EditorSettingsRTTI::instance();
	}

	RTTITypeBase* EditorSettings::getRTTI() const
	{
		return EditorSettings::getRTTIStatic();
	}
}