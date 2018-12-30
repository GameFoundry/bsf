//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Audio/BsAudioListener.h"
#include "Audio/BsAudio.h"
#include "Private/RTTI/BsAudioListenerRTTI.h"

namespace bs
{
	void AudioListener::setVelocity(const Vector3& velocity)
	{
		mVelocity = velocity;
	}

	SPtr<AudioListener> AudioListener::create()
	{
		return gAudio().createListener();
	}	

	RTTITypeBase* AudioListener::getRTTIStatic()
	{
		return AudioListenerRTTI::instance();
	}

	RTTITypeBase* AudioListener::getRTTI() const
	{
		return AudioListener::getRTTIStatic();
	}
}
