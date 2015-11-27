#include "BsPlayInEditorManager.h"

namespace BansheeEngine
{
	PlayInEditorManager::PlayInEditorManager()
		:mState(PlayInEditorState::Stopped), mFrameStepActive(false)
	{ }

	void PlayInEditorManager::setState(PlayInEditorState state)
	{
		// TODO
		//  - make sure to reset the clock when starting/stopping
	}

	void PlayInEditorManager::frameStep()
	{
		// TODO
	}

	void PlayInEditorManager::update()
	{
		// TODO
	}
}