//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAAudioSource.h"
#include "BsOAAudio.h"
#include "AL\al.h"

namespace BansheeEngine
{
	OAAudioSource::OAAudioSource()
	{
		gOAAudio()._registerSource(this);
		rebuild();
	}

	OAAudioSource::~OAAudioSource()
	{
		gOAAudio()._unregisterSource(this);
	}

	void OAAudioSource::setClip(const HAudioClip& clip)
	{
		AudioSource::setClip(clip);

		// TODO
	}

	void OAAudioSource::setPosition(const Vector3& position)
	{
		AudioSource::setPosition(position);

		// TODO
	}

	void OAAudioSource::setVelocity(const Vector3& velocity)
	{
		AudioSource::setVelocity(velocity);

		// TODO
	}

	void OAAudioSource::setVolume(float volume)
	{
		AudioSource::setVolume(volume);

		// TODO
	}

	void OAAudioSource::setPitch(float pitch)
	{
		AudioSource::setPitch(pitch);

		// TODO
	}

	void OAAudioSource::setIsLooping(bool loop)
	{
		AudioSource::setIsLooping(loop);

		// TODO
	}

	void OAAudioSource::setPriority(UINT32 priority)
	{
		AudioSource::setPriority(priority);

		// TODO
	}

	void OAAudioSource::setMinDistance(float distance)
	{
		AudioSource::setMinDistance(distance);

		// TODO
	}

	void OAAudioSource::setAttenuation(float attenuation)
	{
		AudioSource::setAttenuation(attenuation);

		// TODO
	}

	void OAAudioSource::play()
	{
		AudioSource::play();

		// TODO
	}

	void OAAudioSource::pause()
	{
		AudioSource::pause();

		// TODO
	}

	void OAAudioSource::stop()
	{
		AudioSource::stop();

		// TODO
	}

	void OAAudioSource::clear()
	{
		for (auto& source : mSourceIDs)
			alSourcei(source, AL_BUFFER, 0);

		alDeleteSources((UINT32)mSourceIDs.size(), mSourceIDs.data());
		mSourceIDs.clear();
	}

	void OAAudioSource::rebuild()
	{
		auto& contexts = gOAAudio()._getContexts();

		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			// TODO - (Re)apply source settings
		}
	}
}