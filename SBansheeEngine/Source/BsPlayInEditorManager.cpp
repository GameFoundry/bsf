#include "BsPlayInEditorManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsTime.h"

namespace BansheeEngine
{
	PlayInEditorManager::PlayInEditorManager()
		:mState(PlayInEditorState::Stopped), mFrameStepActive(false)
	{ }

	void PlayInEditorManager::setState(PlayInEditorState state)
	{
		if (mState == state)
			return;

		PlayInEditorState oldState = mState;
		mState = state;

		switch (state)
		{
		case PlayInEditorState::Stopped:
		{
			mFrameStepActive = false;
			mPausableTime = 0.0f;
			// TODO - Load saved scene from prefab
		}
			break;
		case PlayInEditorState::Playing:
		{
			if (oldState == PlayInEditorState::Paused)
			{
				ScriptGameObjectManager::instance().sendComponentInitializeEvents();
			}
			else // Was stopped
			{
				// TODO - Save current scene in a prefab
			}
		}
			break;
		case PlayInEditorState::Paused:
		{
			mFrameStepActive = false;
			if (oldState == PlayInEditorState::Stopped)
			{
				// TODO - Save current scene in a prefab
			}
		}
			break;
		default:
			break;
		}		
	}

	void PlayInEditorManager::frameStep()
	{
		switch (mState)
		{
		case PlayInEditorState::Stopped:
		case PlayInEditorState::Paused:
			setState(PlayInEditorState::Playing);
			break;
		}

		mFrameStepActive = true;
	}

	void PlayInEditorManager::update()
	{
		if (mState == PlayInEditorState::Playing)
			mPausableTime += gTime().getFrameDelta();

		// TODO - disable framestep once a frame has passed
	}
}