//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAAudioListener.h"
#include "BsOAAudio.h"
#include "AL/al.h"

namespace BansheeEngine
{
	OAAudioListener::OAAudioListener()
	{
		gOAAudio()._registerListener(this);
		rebuild();
	}

	OAAudioListener::~OAAudioListener()
	{
		gOAAudio()._unregisterListener(this);
	}

	void OAAudioListener::setPosition(const Vector3& position)
	{
		AudioListener::setPosition(position);

		auto& contexts = gOAAudio()._getContexts();
		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			updatePosition();
		}
	}

	void OAAudioListener::setDirection(const Vector3& direction)
	{
		AudioListener::setDirection(direction);

		std::array<float, 6> orientation = getOrientation();
		auto& contexts = gOAAudio()._getContexts();
		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			updateOrientation(orientation);
		}
	}

	void OAAudioListener::setUp(const Vector3& up)
	{
		AudioListener::setUp(up);

		std::array<float, 6> orientation = getOrientation();
		auto& contexts = gOAAudio()._getContexts();
		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			updateOrientation(orientation);
		}
	}

	void OAAudioListener::setVelocity(const Vector3& velocity)
	{
		AudioListener::setVelocity(velocity);

		auto& contexts = gOAAudio()._getContexts();
		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			updateVelocity();
		}
	}

	void OAAudioListener::rebuild()
	{
		auto& contexts = gOAAudio()._getContexts();

		float globalVolume = gAudio().getVolume();
		std::array<float, 6> orientation = getOrientation();

		for (auto& context : contexts)
		{
			if (contexts.size() > 1) // If only one context is available it is guaranteed it is always active, so we can avoid setting it
				alcMakeContextCurrent(context);

			updatePosition();
			updateOrientation(orientation);
			updateVelocity();
			updateVolume(globalVolume);
		}
	}

	std::array<float, 6> OAAudioListener::getOrientation() const
	{
		return
		{
			mDirection.x,
			mDirection.y,
			mDirection.z,
			mUp.x,
			mUp.y,
			mUp.z
		};
	}

	void OAAudioListener::updatePosition()
	{
		alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
	}

	void OAAudioListener::updateOrientation(const std::array<float, 6>& orientation)
	{
		alListenerfv(AL_ORIENTATION, orientation.data());
	}

	void OAAudioListener::updateVelocity()
	{
		alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	}

	void OAAudioListener::updateVolume(float volume)
	{
		alListenerf(AL_GAIN, volume);
	}
}