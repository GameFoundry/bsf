//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAudioSource.h"
#include "BsAudio.h"

namespace BansheeEngine
{
	AudioSource::AudioSource()
		: mVolume(1.0f), mPitch(1.0f), mLoop(false), mPriority(0), mMinDistance(1.0f), mAttenuation(1.0f)
		, mState(AudioSourceState::Stopped)
	{

	}

	void AudioSource::setClip(const HAudioClip& clip)
	{
		mAudioClip = clip;
	}

	void AudioSource::setPosition(const Vector3& position)
	{
		mPosition = position;
	}

	void AudioSource::setVelocity(const Vector3& velocity)
	{
		mVelocity = velocity;
	}

	void AudioSource::setVolume(float volume)
	{
		mVolume = volume;
	}

	void AudioSource::setPitch(float pitch)
	{
		mPitch = pitch;
	}

	void AudioSource::setIsLooping(bool loop)
	{
		mLoop = loop;
	}

	void AudioSource::setPriority(UINT32 priority)
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

	void AudioSource::play()
	{
		mState = AudioSourceState::Playing;
	}

	void AudioSource::pause()
	{
		mState = AudioSourceState::Paused;
	}

	void AudioSource::stop()
	{
		mState = AudioSourceState::Stopped;
	}

	SPtr<AudioSource> AudioSource::create()
	{
		return gAudio().createSource();
	}
}