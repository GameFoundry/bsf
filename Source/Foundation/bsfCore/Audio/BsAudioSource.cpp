//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Audio/BsAudioSource.h"
#include "Audio/BsAudio.h"
#include "Math/BsMath.h"
#include "Private/RTTI/BsAudioSourceRTTI.h"

namespace bs
{
	void AudioSource::setClip(const HAudioClip& clip)
	{
		mAudioClip = clip;

		markListenerResourcesDirty();
	}

	void AudioSource::setVelocity(const Vector3& velocity)
	{
		mVelocity = velocity;
	}

	void AudioSource::setVolume(float volume)
	{
		mVolume = Math::clamp01(volume);
	}

	void AudioSource::setPitch(float pitch)
	{
		mPitch = pitch;
	}

	void AudioSource::setIsLooping(bool loop)
	{
		mLoop = loop;
	}

	void AudioSource::setPriority(INT32 priority)
	{
		mPriority = priority;
	}

	void AudioSource::setMinDistance(float distance)
	{
		mMinDistance = distance;
	}

	void AudioSource::setAttenuation(float attenuation)
	{
		mAttenuation = attenuation;
	}

	SPtr<AudioSource> AudioSource::create()
	{
		return gAudio().createSource();
	}

	void AudioSource::getListenerResources(Vector<HResource>& resources)
	{
		if (mAudioClip != nullptr)
			resources.push_back(mAudioClip);
	}

	void AudioSource::notifyResourceChanged(const HResource& resource)
	{
		onClipChanged();
	}

	RTTITypeBase* AudioSource::getRTTIStatic()
	{
		return AudioSourceRTTI::instance();
	}

	RTTITypeBase* AudioSource::getRTTI() const
	{
		return AudioSource::getRTTIStatic();
	}
}
