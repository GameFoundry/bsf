//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	enum class AudioSourceState
	{
		Playing,
		Paused,
		Stopped
	};

	class BS_CORE_EXPORT AudioSource
	{
	public:
		virtual ~AudioSource() { }

		virtual void setClip(const HAudioClip& clip);
		HAudioClip getClip() const { return mAudioClip; }

		virtual void setPosition(const Vector3& position);
		Vector3 getPosition() const { return mPosition; }

		virtual void setVelocity(const Vector3& velocity);
		Vector3 getVelocity() const { return mVelocity; }

		virtual void setVolume(float volume); // [0, 1] range
		float getVolume() const { return mVolume; }

		virtual void setPitch(float pitch);
		float getPitch() const { return mPitch; }

		virtual void setIsLooping(bool loop);
		bool getIsLooping() const { return mLoop; }

		virtual void setPriority(UINT32 priority);
		UINT32 getPriority() const { return mPriority; }

		virtual void setMinDistance(float distance);
		float getMinDistance() const { return mMinDistance; }

		virtual void setAttenuation(float attenuation);
		float getAttenuation() const { return mAttenuation; }

		virtual void play();
		virtual void pause();
		virtual void stop();

		AudioSourceState getState() const { return mState; }

		static SPtr<AudioSource> create();

	protected:
		AudioSource();

		HAudioClip mAudioClip;
		Vector3 mPosition;
		Vector3 mVelocity;
		float mVolume;
		float mPitch;
		bool mLoop;
		UINT32 mPriority;
		float mMinDistance;
		float mAttenuation;
		AudioSourceState mState;
	};

	/** @} */
}