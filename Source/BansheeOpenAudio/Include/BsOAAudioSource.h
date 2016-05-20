//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsOAPrerequisites.h"
#include "BsAudioSource.h"

namespace BansheeEngine
{
	/** @addtogroup OpenAudio
	 *  @{
	 */
	
	/** OpenAL implementation of an AudioSource. */
	class BS_OA_EXPORT OAAudioSource : public AudioSource
	{
	public:
		OAAudioSource();
		virtual ~OAAudioSource();

		/** @copydoc AudioSource::setClip */
		void setClip(const HAudioClip& clip) override;

		/** @copydoc AudioSource::setPosition */
		void setPosition(const Vector3& position) override;

		/** @copydoc AudioSource::setVelocity */
		void setVelocity(const Vector3& velocity) override;

		/** @copydoc AudioSource::setVolume */
		void setVolume(float volume) override;

		/** @copydoc AudioSource::setPitch */
		void setPitch(float pitch) override;

		/** @copydoc AudioSource::setIsLooping */
		void setIsLooping(bool loop) override;

		/** @copydoc AudioSource::setPriority */
		void setPriority(UINT32 priority) override;

		/** @copydoc AudioSource::setMinDistance */
		void setMinDistance(float distance) override;

		/** @copydoc AudioSource::setAttenuation */
		void setAttenuation(float attenuation) override;

		/** @copydoc AudioSource::play */
		void play() override;

		/** @copydoc AudioSource::pause */
		void pause() override;

		/** @copydoc AudioSource::stop */
		void stop() override;

	private:
		friend class OAAudio;

		/** Destroys the internal representation of the audio source. */
		void clear();

		/** Rebuilds the internal representation of an audio source. */
		void rebuild();

		Vector<UINT32> mSourceIDs;
	};

	/** @} */
}